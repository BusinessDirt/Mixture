#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraph.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace Mixture
{
    bool RenderGraphAlgorithms::SortPasses(Vector<RGPassNode>& passes)
    {
        const size_t passCount = passes.size();
        if (passCount < 2) return true;

        Vector<Vector<size_t>> adjacencyList(passCount);
        Vector<std::unordered_set<size_t>> adjacencySets(passCount);
        Vector<size_t> inDegree(passCount, 0);

        std::unordered_map<RGResourceHandle::IDType, size_t> lastWriters;
        std::unordered_map<RGResourceHandle::IDType, Vector<size_t>> readersSinceLastWrite;

        auto addDependency = [&](size_t before, size_t after)
        {
            if (before == after) return;

            if (adjacencySets[before].insert(after).second)
            {
                adjacencyList[before].push_back(after);
                ++inDegree[after];
            }
        };

        auto registerRead = [&](RGResourceHandle handle, size_t passIndex)
        {
            const auto writer = lastWriters.find(handle.ID);
            if (writer != lastWriters.end())
            {
                // Read-after-write (RAW).
                addDependency(writer->second, passIndex);
            }

            auto& readers = readersSinceLastWrite[handle.ID];
            if (std::find(readers.begin(), readers.end(), passIndex) == readers.end())
            {
                readers.push_back(passIndex);
            }
        };

        auto registerWrite = [&](RGResourceHandle handle, size_t passIndex)
        {
            const auto writer = lastWriters.find(handle.ID);
            if (writer != lastWriters.end())
            {
                // Write-after-write (WAW).
                addDependency(writer->second, passIndex);
            }

            auto& readers = readersSinceLastWrite[handle.ID];
            for (const size_t reader : readers)
            {
                // Write-after-read (WAR).
                addDependency(reader, passIndex);
            }
            readers.clear();

            lastWriters[handle.ID] = passIndex;
        };

        for (size_t passIndex = 0; passIndex < passCount; ++passIndex)
        {
            const auto& pass = passes[passIndex];

            for (const auto handle : pass.Reads)
            {
                registerRead(handle, passIndex);
            }
            for (const auto& write : pass.Writes)
            {
                registerWrite(write.Handle, passIndex);
            }
            for (const auto handle : pass.BufferWrites)
            {
                registerWrite(handle, passIndex);
            }
        }

        // Always choose the earliest declared ready pass. This keeps independent
        // passes stable while still honoring every resource dependency.
        std::priority_queue<size_t, Vector<size_t>, std::greater<size_t>> readyPasses;
        for (size_t passIndex = 0; passIndex < passCount; ++passIndex)
        {
            if (inDegree[passIndex] == 0) readyPasses.push(passIndex);
        }

        Vector<size_t> sortedIndices;
        sortedIndices.reserve(passCount);

        while (!readyPasses.empty())
        {
            const size_t passIndex = readyPasses.top();
            readyPasses.pop();
            sortedIndices.push_back(passIndex);

            for (const size_t dependentPass : adjacencyList[passIndex])
            {
                if (--inDegree[dependentPass] == 0)
                {
                    readyPasses.push(dependentPass);
                }
            }
        }

        if (sortedIndices.size() != passCount) return false;

        Vector<RGPassNode> sortedPasses;
        sortedPasses.reserve(passCount);
        for (const size_t passIndex : sortedIndices)
        {
            sortedPasses.push_back(std::move(passes[passIndex]));
        }
        passes = std::move(sortedPasses);
        return true;
    }

    void RenderGraph::Clear()
    {
        m_PassAllocator.Reset();
        m_Passes.clear();
        m_Resources.clear();
        m_Registry.Clear();
        // Note: m_Cache is NOT cleared here, to support persistence across frames.
    }

    void RenderGraph::Compile()
    {
        SortPasses();
        CalculateLifetimes();
        CalculateBarriers();

#ifdef OPAL_DEBUG
        DumpGraphToJSON();
#endif
    }

    void RenderGraph::Execute(RHI::ICommandList* cmdList, RHI::IGraphicsContext* context)
    {
        // Realize Resources (Allocation Phase)
        for (const auto& node : m_Resources)
        {
            if (node.Type == RGResourceType::ImportedTexture)
            {
                m_Registry.ImportTexture(node.Handle, node.ExternalTexture);
            }
            else if (node.Type == RGResourceType::ImportedBuffer)
            {
                m_Registry.ImportBuffer(node.Handle, node.ExternalBuffer);
            }
            else if (node.Type == RGResourceType::Texture)
            {
                // Get from Cache
                auto texture = m_Cache.GetOrCreateTexture(node.Name, node.TextureDesc);
                m_Registry.RegisterTexture(node.Handle, texture.get());
            }
            else if (node.Type == RGResourceType::Buffer)
            {
                // Get from Cache
                auto buffer = m_Cache.GetOrCreateBuffer(node.Name, node.BufferDesc);
                m_Registry.RegisterBuffer(node.Handle, buffer.get());
            }
        }

        // Execute Passes
        for (const auto& pass : m_Passes)
        {
            // Execute Barriers
            for (const auto& barrier : pass.Barriers)
            {
                auto& resNode = m_Resources[barrier.Resource.ID];
                if (resNode.Type == RGResourceType::Texture || resNode.Type == RGResourceType::ImportedTexture)
                {
                    cmdList->PipelineBarrier(
                        m_Registry.GetTexture(barrier.Resource),
                        barrier.Before,
                        barrier.After
                    );
                }
                else
                {
                    // Buffer Barriers
                    // TODO: cmdList->BufferBarrier(...)
                }
            }

            // Run Logic
            if (pass.Execute)
            {
                RHI::RenderingInfo renderingInfo;

                // Setup Render Area from Attachment Writes
                if (!pass.Writes.empty())
                {
                    RHI::ITexture* firstTex = m_Registry.GetTexture(pass.Writes[0].Handle);
                    if (firstTex)
                    {
                        renderingInfo.RenderAreaWidth = firstTex->GetWidth();
                        renderingInfo.RenderAreaHeight = firstTex->GetHeight();
                    }
                }
                else
                {
                    // Fallback
                    renderingInfo.RenderAreaWidth = context->GetSwapchainWidth();
                    renderingInfo.RenderAreaHeight = context->GetSwapchainHeight();
                }

                // Populate Attachments
                RHI::RenderingAttachment depthAttachmentTemp;
                renderingInfo.DepthAttachment = nullptr;

                for (const auto& write : pass.Writes)
                {
                    // Look up the actual RHI Texture from the handle
                    auto texture = m_Registry.GetTexture(write.Handle);

                    // Create the attachment struct
                    RHI::RenderingAttachment attachment;
                    attachment.Image = texture;
                    attachment.LoadOp = write.LoadOp;
                    attachment.StoreOp = write.StoreOp;

                    // Copy Clear Color
                    memcpy(attachment.ClearColor, write.ClearColor, sizeof(float) * 4);
                    attachment.DepthClearValue = write.DepthClearValue;

                    // Sort into Color vs Depth
                    if (IsDepthFormat(texture->GetFormat()))
                    {
                        depthAttachmentTemp = attachment;
                        renderingInfo.DepthAttachment = &depthAttachmentTemp;
                    }
                    else
                    {
                        renderingInfo.ColorAttachments.push_back(attachment);
                    }
                }

                // Only call BeginRendering if we have attachments
                if (!renderingInfo.ColorAttachments.empty() || renderingInfo.DepthAttachment)
                {
                    uint32_t width = renderingInfo.RenderAreaWidth;
                    uint32_t height = renderingInfo.RenderAreaHeight;

                    cmdList->BeginRendering(renderingInfo);
                    cmdList->SetViewport(0, 0, (float)width, (float)height);
                    cmdList->SetScissor(0, 0, width, height);

                    pass.Execute(m_Registry, cmdList); // Draw commands happen here

                    cmdList->EndRendering();
                }
                else
                {
                    // Just execute without dynamic rendering scope (Compute, Copy, etc.)
                    pass.Execute(m_Registry, cmdList);
                }
            }
        }
    }

    RGResourceHandle RenderGraph::ImportResource(const std::string& name, RHI::ITexture* resource)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::ImportedTexture;

        node.TextureDesc.Width = resource->GetWidth();
        node.TextureDesc.Height = resource->GetHeight();
        node.TextureDesc.PixelFormat = resource->GetFormat();
        node.TextureDesc.InitialState = RHI::ResourceState::Undefined; // We don't track external state yet

        node.ExternalTexture = resource;

        m_Resources.push_back(node);
        m_Registry.ImportTexture(handle, resource);

        return handle;
    }

    RGResourceHandle RenderGraph::ImportResource(const std::string& name, RHI::IBuffer* resource)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::ImportedBuffer;

        node.BufferDesc.Size = resource->GetSize();
        node.BufferDesc.Usage = resource->GetUsage();

        node.ExternalBuffer = resource;

        m_Resources.push_back(node);
        m_Registry.ImportBuffer(handle, resource);

        return handle;
    }

    RGResourceHandle RenderGraph::CreateResource(const std::string& name, const RHI::TextureDesc& desc)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::Texture;
        node.TextureDesc = desc;

        m_Resources.push_back(node);
        return handle;
    }

    RGResourceHandle RenderGraph::CreateResource(const std::string& name, const RHI::BufferDesc& desc)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::Buffer;
        node.BufferDesc = desc;

        m_Resources.push_back(node);
        return handle;
    }

    RGResourceHandle RenderGraph::GetResource(const std::string& name) const
    {
        for (const auto& node : m_Resources)
        {
            if (node.Name == name)
            {
                return node.Handle;
            }
        }

        OPAL_ERROR("Core/RenderGraph", "Resource not found: %s", name.c_str());
        return RGResourceHandle();
    }

    const RHI::TextureDesc& RenderGraph::GetTextureDesc(RGResourceHandle handle) const
    {
        OPAL_ASSERT("Core/RenderGraph", handle.IsValid() && handle.ID < m_Resources.size(), "Invalid Handle!");
        // TODO: Assert type
        return m_Resources[handle.ID].TextureDesc;
    }

    const RHI::BufferDesc& RenderGraph::GetBufferDesc(RGResourceHandle handle) const
    {
        OPAL_ASSERT("Core/RenderGraph", handle.IsValid() && handle.ID < m_Resources.size(), "Invalid Handle!");
        // TODO: Assert type
        return m_Resources[handle.ID].BufferDesc;
    }

    const RGResourceNode& RenderGraph::GetResourceNode(RGResourceHandle handle) const
    {
        OPAL_ASSERT("Core/RenderGraph", handle.IsValid() && handle.ID < m_Resources.size(), "Invalid Handle!");
        return m_Resources[handle.ID];
    }

    void RenderGraph::SortPasses()
    {
        if (!RenderGraphAlgorithms::SortPasses(m_Passes))
        {
            OPAL_ERROR("Core/RenderGraph", "Failed to produce a valid pass ordering.");
        }
    }

    void RenderGraph::CalculateLifetimes()
    {
        for (auto& node : m_Resources)
        {
            node.FirstPassIndex = -1;
            node.LastPassIndex = -1;
        }

        for (int32_t passIndex = 0; passIndex < static_cast<int32_t>(m_Passes.size()); ++passIndex)
        {
            const auto& pass = m_Passes[passIndex];

            auto UpdateResource = [&](RGResourceHandle handle)
            {
                if (!handle.IsValid()) return;
                auto& node = m_Resources[handle.ID];

                if (node.FirstPassIndex == -1) node.FirstPassIndex = passIndex;
                node.LastPassIndex = passIndex;
            };

            for (auto handle : pass.Reads) UpdateResource(handle);
            for (auto write : pass.Writes) UpdateResource(write.Handle);
            for (auto handle : pass.BufferWrites) UpdateResource(handle);
        }
    }

    void RenderGraph::CalculateBarriers()
    {
        Vector<RHI::ResourceState> currentStates(m_Resources.size());
        Vector<bool> wasLastWrite(m_Resources.size(), false);

        for (size_t i = 0; i < m_Resources.size(); ++i)
        {
            if (m_Resources[i].Type == RGResourceType::Texture || m_Resources[i].Type == RGResourceType::ImportedTexture)
                currentStates[i] = m_Resources[i].TextureDesc.InitialState;
            else
                currentStates[i] = RHI::ResourceState::Undefined;
        }

        for (auto& pass : m_Passes)
        {
            pass.Barriers.clear();

            auto TransitionResource = [&](RGResourceHandle handle, RHI::ResourceState targetState, bool isWrite)
            {
                uint32_t id = handle.ID;
                RHI::ResourceState current = currentStates[id];
                bool previousWasWrite = wasLastWrite[id];

                bool layoutChanged = (current != targetState);
                bool hazardExists = previousWasWrite || (isWrite && current != RHI::ResourceState::Undefined);

                if (!isWrite && !previousWasWrite && !layoutChanged) return;

                if (layoutChanged || hazardExists)
                {
                    RGBarrier barrier;
                    barrier.Resource = handle;
                    barrier.Before = current;
                    barrier.After = targetState;
                    pass.Barriers.push_back(barrier);
                    currentStates[id] = targetState;
                }
                wasLastWrite[id] = isWrite;
            };

            for (auto& handle : pass.Reads)
            {
                auto& node = m_Resources[handle.ID];
                if (node.Type == RGResourceType::Texture || node.Type == RGResourceType::ImportedTexture)
                    TransitionResource(handle, RHI::ResourceState::ShaderResource, false);
                else
                    TransitionResource(handle, RHI::ResourceState::ShaderResource, false); // Or Vertex/Constant based on usage?
            }

            for (auto& info : pass.Writes)
            {
                RGResourceHandle handle = info.Handle;
                auto& node = m_Resources[handle.ID];

                if (node.Type == RGResourceType::Texture || node.Type == RGResourceType::ImportedTexture)
                {
                    bool isDepth = IsDepthFormat(node.TextureDesc.PixelFormat);
                    RHI::ResourceState target = isDepth ? RHI::ResourceState::DepthStencilWrite : RHI::ResourceState::RenderTarget;
                    TransitionResource(handle, target, true);
                }
            }

            for (auto& handle : pass.BufferWrites)
            {
                 TransitionResource(handle, RHI::ResourceState::UnorderedAccess, true);
            }
        }
    }

    void RenderGraph::DumpGraphToJSON()
    {
        static bool executed = false;
        if (executed) return;

        // Find the project root by looking for ".git"
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::filesystem::path projectRoot = currentPath;
        bool foundGit = false;

        while (true)
        {
            if (std::filesystem::exists(projectRoot / ".git"))
            {
                foundGit = true;
                break;
            }

            // Move up one level
            if (projectRoot.has_parent_path() && projectRoot != projectRoot.parent_path())
            {
                projectRoot = projectRoot.parent_path();
            }
            else
            {
                break; // Reached system root (e.g., C:\ or /)
            }
        }

        // Construct the target path
        std::filesystem::path outputDir;

        if (foundGit)
        {
            outputDir = projectRoot / "docs" / "visualizers";
        }
        else
        {
            OPAL_WARN("Core/RenderGraph", ".git directory not found. Saving to build directory.");
            outputDir = currentPath / "docs" / "visualizers";
        }

        // Create the 'docs' directory if it doesn't exist
        if (!std::filesystem::exists(outputDir))
        {
            std::filesystem::create_directories(outputDir);
        }

        std::ofstream out(outputDir / "graph.json");

        out << "{\n";
        out << "  \"resources\": [\n";
        for (size_t i = 0; i < m_Resources.size(); ++i)
        {
            std::string name = m_Resources[i].Name.empty() ? "Res_" + std::to_string(i) : m_Resources[i].Name;
            std::string typeStr = (m_Resources[i].Type == RGResourceType::Texture) ? "Texture" : "Buffer";
            out << "    { \"id\": " << i << ", \"name\": \"" << name << "\", \"type\": \"" << typeStr << "\" }";
            if (i < m_Resources.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ],\n";

        out << "  \"passes\": [\n";
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            const auto& pass = m_Passes[i];
            out << "    {\n";
            out << "      \"id\": " << i << ",\n";
            out << "      \"name\": \"" << pass.Name << "\",\n";

            // --- NEW: Dump Barriers ---
            out << "      \"barriers\": [\n";
            for (size_t b = 0; b < pass.Barriers.size(); ++b)
            {
                const auto& barrier = pass.Barriers[b];
                std::string_view fromState = RHI::ToString(barrier.Before);
                std::string_view toState   = RHI::ToString(barrier.After);

                out << "        {";
                out << " \"res\": " << barrier.Resource.ID << ",";
                out << " \"from\": \"" << fromState << "\",";
                out << " \"to\": \"" << toState << "\"";
                out << " }";

                if (b < pass.Barriers.size() - 1) out << ",";
                out << "\n";
            }
            out << "      ],\n";

            // Writes
            out << "      \"writes\": [";
            for (size_t k = 0; k < pass.Writes.size(); ++k) {
                out << pass.Writes[k].Handle.ID;
                if (k < pass.Writes.size() - 1) out << ", ";
            }
            out << "],\n";

            // Reads
            out << "      \"reads\": [";
            for (size_t k = 0; k < pass.Reads.size(); ++k) {
                out << pass.Reads[k].ID;
                if (k < pass.Reads.size() - 1) out << ", ";
            }
            out << "]\n";

            out << "    }";
            if (i < m_Passes.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ]\n";
        out << "}\n";
        out << std::flush;

        OPAL_LOG_DEBUG("Core/RenderGraph", "Dumped Graph to JSON file: {}/graph.json", outputDir.string());
        executed = true;
    }
}
