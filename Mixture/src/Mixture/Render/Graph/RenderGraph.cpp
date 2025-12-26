#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraph.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

#include <filesystem>
#include <fstream>

namespace Mixture
{
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
        node.TextureDesc.Format = resource->GetFormat();
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
        size_t passCount = m_Passes.size();
        if (passCount == 0) return;

        // Build Adjacency List
        Vector<Vector<size_t>> adjacencyList(passCount);
        Vector<int> inDegree(passCount, 0);

        // Track who wrote to a resource last (ResourceID -> PassIndex)
        std::unordered_map<uint32_t, size_t> resourceWriters;

        for (size_t i = 0; i < passCount; ++i)
        {
            auto& pass = m_Passes[i];

            // DEPENDENCY: Read-after-Write (RAW)
            for (auto& handle : pass.Reads)
            {
                if (resourceWriters.find(handle.ID) != resourceWriters.end())
                {
                    size_t writerIndex = resourceWriters[handle.ID];
                    adjacencyList[writerIndex].push_back(i);
                    inDegree[i]++;
                }
            }

            // DEPENDENCY: Write-after-Write (WAW) - Attachments
            for (auto& write : pass.Writes)
            {
                if (resourceWriters.find(write.Handle.ID) != resourceWriters.end())
                {
                    size_t writerIndex = resourceWriters[write.Handle.ID];
                    if (writerIndex != i)
                    {
                        adjacencyList[writerIndex].push_back(i);
                        inDegree[i]++;
                    }
                }
            }
            
            // DEPENDENCY: Write-after-Write (WAW) - Buffers
            for (auto& handle : pass.BufferWrites)
            {
                if (resourceWriters.find(handle.ID) != resourceWriters.end())
                {
                    size_t writerIndex = resourceWriters[handle.ID];
                    if (writerIndex != i)
                    {
                        adjacencyList[writerIndex].push_back(i);
                        inDegree[i]++;
                    }
                }
            }

            // UPDATE writers
            for (auto& write : pass.Writes) resourceWriters[write.Handle.ID] = i;
            for (auto& handle : pass.BufferWrites) resourceWriters[handle.ID] = i;
        }

        // Kahn's Algorithm
        Vector<size_t> queue;
        for (size_t i = 0; i < passCount; ++i)
            if (inDegree[i] == 0) queue.push_back(i);

        Vector<RGPassNode> sortedPasses;
        sortedPasses.reserve(passCount);

        while (!queue.empty())
        {
            size_t u = queue.back();
            queue.pop_back();

            sortedPasses.push_back(m_Passes[u]);

            for (size_t v : adjacencyList[u])
            {
                inDegree[v]--;
                if (inDegree[v] == 0) queue.push_back(v);
            }
        }

        if (sortedPasses.size() != passCount)
        {
            OPAL_ERROR("Core/RenderGraph", "Cyclic pass dependency detected.");
            return;
        }

        m_Passes = std::move(sortedPasses);
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
                    bool isDepth = IsDepthFormat(node.TextureDesc.Format);
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
        // Construct the target path
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::filesystem::path outputDir = currentPath / "docs" / "visualizers";
        if (!std::filesystem::exists(outputDir)) std::filesystem::create_directories(outputDir);
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
            
             // Reads
            out << "      \"reads\": [";
            for (size_t k = 0; k < pass.Reads.size(); ++k) {
                out << pass.Reads[k].ID;
                if (k < pass.Reads.size() - 1) out << ", ";
            }
            out << "],\n";
            
            // Writes (Attachments + Buffers)
            out << "      \"writes\": [";
            for (size_t k = 0; k < pass.Writes.size(); ++k) {
                out << pass.Writes[k].Handle.ID;
                bool isLastAttachment = (k == pass.Writes.size() - 1);
                if (!isLastAttachment || !pass.BufferWrites.empty()) out << ", ";
            }
            for (size_t k = 0; k < pass.BufferWrites.size(); ++k) {
                out << pass.BufferWrites[k].ID;
                if (k < pass.BufferWrites.size() - 1) out << ", ";
            }
            out << "]\n";

            out << "    }";
            if (i < m_Passes.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ]\n";
        out << "}\n";
    }
}
