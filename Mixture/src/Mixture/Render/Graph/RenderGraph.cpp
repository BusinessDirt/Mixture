#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraph.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

#include <filesystem>

namespace Mixture
{
    void RenderGraph::Clear()
    {
        m_PassAllocator.Reset();
        m_Passes.clear();
        m_Resources.clear();
        m_Registry.Clear();
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
            if (!node.IsImported)
            {
                auto texture = context->GetDevice().CreateTexture(node.Desc);
                m_Registry.ImportTexture(node.Handle, texture.get());
            }

            // Imported resources are already in m_Registry via ImportResource()
        }

        // Execute Passes
        for (const auto& pass : m_Passes)
        {

            // Execute Barriers
            for (const auto& barrier : pass.Barriers)
            {
                 cmdList->PipelineBarrier(
                     m_Registry.GetTexture(barrier.Resource), // Look up in member registry
                     barrier.Before,
                     barrier.After
                 );
            }

            // Run Logic
            if (pass.Execute)
            {
                RHI::RenderingInfo renderingInfo;

                // Setup Render Area
                // Ideally, get this from the first output texture's dimensions
                if (!pass.Writes.empty())
                {
                    auto firstTex = m_Registry.GetTexture(pass.Writes[0].Handle);
                    renderingInfo.RenderAreaWidth = firstTex->GetWidth();
                    renderingInfo.RenderAreaHeight = firstTex->GetHeight();
                }
                else
                {
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

                // Only call BeginRendering if we actually have attachments (Raster Pass)
                if (!renderingInfo.ColorAttachments.empty() || renderingInfo.DepthAttachment)
                {
                    uint32_t width = Application::Get().GetContext().GetSwapchainWidth();
                    uint32_t height = Application::Get().GetContext().GetSwapchainHeight();
                    cmdList->BeginRendering(renderingInfo);
                    cmdList->SetViewport(0, 0, width, height);
                    cmdList->SetScissor(0, 0, width, height);
                    pass.Execute(m_Registry, cmdList); // Draw commands happen here
                    cmdList->EndRendering();
                }
                else
                {
                    // If no attachments, it might be a Compute Pass or Copy Pass
                    // Just execute without dynamic rendering scope
                    pass.Execute(m_Registry, cmdList);
                }
            }
        }
    }

    RGResourceHandle RenderGraph::ImportResource(const std::string& name, RHI::ITexture* resource)
    {
        // Create a handle/node just like a normal resource
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGTextureNode node;
        node.Handle = handle;
        node.Name = name;
        node.Desc.Width = resource->GetWidth();   // Extract info from the real object
        node.Desc.Height = resource->GetHeight();
        node.Desc.Format = resource->GetFormat();
        node.Desc.InitialState = RHI::ResourceState::Undefined;
        node.IsImported = true;
        node.ExternalTexture = resource; // Keep it safe

        m_Resources.push_back(node);

        // Immediately register it so it's available for execution
        m_Registry.ImportTexture(handle, resource);

        return handle;
    }

    RGResourceHandle RenderGraph::CreateResource(const std::string& name, const RHI::TextureDesc& desc)
    {
        // Create a new ID based on the current size of the vector
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        // Create the Node storage
        RGTextureNode node;
        node.Handle = handle;
        node.Name = name;
        node.Desc = desc;

        // Store it
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

        OPAL_ERROR("Core/RenderGraph", "Resource not found: {}", name);
        return RGResourceHandle();
    }

    void RenderGraph::SortPasses()
    {
        size_t passCount = m_Passes.size();

        // Build Adjacency List
        Vector<Vector<size_t>> adjacencyList(passCount);
        Vector<int> inDegree(passCount, 0);

        // Track who wrote to a resource last (ResourceID -> PassIndex)
        std::unordered_map<uint32_t, size_t> resourceWriters;

        for (size_t i = 0; i < passCount; ++i)
        {
            auto& pass = m_Passes[i];

            // DEPENDENCY: Read-after-Write (RAW)
            // If this pass reads 'TexA', it depends on whoever wrote 'TexA' last.
            for (auto& handle : pass.Reads)
            {
                if (resourceWriters.find(handle.ID) != resourceWriters.end())
                {
                    size_t writerIndex = resourceWriters[handle.ID];

                    // Add edge: Writer -> Current Pass
                    adjacencyList[writerIndex].push_back(i);
                    inDegree[i]++;
                }
            }

            // DEPENDENCY: Write-after-Write (WAW)
            // If we write to something already written to this frame, we must run AFTER the previous writer.
            for (auto& write : pass.Writes)
            {
                if (resourceWriters.find(write.Handle.ID) != resourceWriters.end())
                {
                    size_t writerIndex = resourceWriters[write.Handle.ID];

                    // Prevent self-dependency if a pass writes to the same subresource multiple times
                    if (writerIndex != i)
                    {
                        adjacencyList[writerIndex].push_back(i);
                        inDegree[i]++;
                    }
                }
            }

            // UPDATE: This pass is now the latest writer for its outputs
            for (auto& write : pass.Writes)
                resourceWriters[write.Handle.ID] = i;
        }

        // Kahn's Algorithm (Standard Topological Sort)
        Vector<size_t> queue; // Passes with 0 dependencies
        for (size_t i = 0; i < passCount; ++i)
            if (inDegree[i] == 0) queue.push_back(i);

        Vector<RGPassNode> sortedPasses;
        sortedPasses.reserve(passCount);

        while (!queue.empty())
        {
            size_t u = queue.back();
            queue.pop_back();

            // Add to final sorted list
            sortedPasses.push_back(m_Passes[u]);

            // "Remove" this pass and satisfy dependencies
            for (size_t v : adjacencyList[u])
            {
                inDegree[v]--;
                if (inDegree[v] == 0) queue.push_back(v);
            }
        }

        // Cycle Detection: If sorted list is smaller, we have a circular dependency
        if (sortedPasses.size() != passCount)
        {
            OPAL_ERROR("Core/RenderGraph", "Cyclic pass dependency detected.");
            return;
        }

        m_Passes = std::move(sortedPasses);
    }

    void RenderGraph::CalculateLifetimes()
    {
        // Reset all lifetimes
        for (auto& node : m_Resources)
        {
            node.FirstPassIndex = -1;
            node.LastPassIndex = -1;
        }

        // Iterate through passes in execution order
        for (int32_t passIndex = 0; passIndex < static_cast<int32_t>(m_Passes.size()); ++passIndex)
        {
            const auto& pass = m_Passes[passIndex];

            // Helper lambda to update a single resource
            auto UpdateResource = [&](RGResourceHandle handle)
            {
                if (!handle.IsValid()) return;

                auto& node = m_Resources[handle.ID];

                // If this is the first time we've seen it, mark start
                if (node.FirstPassIndex == -1)
                {
                    node.FirstPassIndex = passIndex;
                }

                // Always update the end to the current pass
                node.LastPassIndex = passIndex;
            };

            // Check Reads
            for (auto handle : pass.Reads)
            {
                UpdateResource(handle);
            }

            // Check Writes
            for (auto write : pass.Writes)
            {
                UpdateResource(write.Handle);
            }
        }
    }

    void RenderGraph::CalculateBarriers()
    {
        Vector<RHI::ResourceState> currentStates(m_Resources.size());
        Vector<bool> wasLastWrite(m_Resources.size(), false);

        for (size_t i = 0; i < m_Resources.size(); ++i)
        {
            // For internal resources, 'Undefined' is fine.
            // For imported ones, use their real state.
            currentStates[i] = m_Resources[i].Desc.InitialState;
        }

        for (auto& pass : m_Passes)
        {
            pass.Barriers.clear();

            // --- Helper Lambda to Inject Barrier ---
            auto TransitionResource = [&](RGResourceHandle handle, RHI::ResourceState targetState, bool isWrite)
            {
                uint32_t id = handle.ID;
                RHI::ResourceState current = currentStates[id];
                bool previousWasWrite = wasLastWrite[id];

                // We need a barrier if:
                // 1. The Layout/State needs to change (e.g. SRV -> RTV)
                // 2. OR: The previous access was a Write (Hazard: RAW or WAW)
                // 3. OR: The current access is a Write AND previous was Read (Hazard: WAR)
                //    (Note: WAR is often handled implicitly by subpass deps, but explicit barriers are safer)

                bool layoutChanged = (current != targetState);
                bool hazardExists = previousWasWrite || (isWrite && current != RHI::ResourceState::Undefined);

                // Optimization: Read-after-Read (RAR) usually needs no barrier.
                if (!isWrite && !previousWasWrite && !layoutChanged)
                {
                    return;
                }

                if (layoutChanged || hazardExists)
                {
                    RGBarrier barrier;
                    barrier.Resource = handle;
                    barrier.Before = current;
                    barrier.After = targetState;

                    // Add to the pass
                    pass.Barriers.push_back(barrier);

                    // Update tracking
                    currentStates[id] = targetState;
                }

                wasLastWrite[id] = isWrite;
            };

            // --- Process Reads ---
            for (auto& handle : pass.Reads)
            {
                TransitionResource(handle, RHI::ResourceState::ShaderResource, false);
            }

            // --- Process Writes ---
            for (auto& info : pass.Writes)
            {
                RGResourceHandle handle = info.Handle;

                // Determine Target State
                bool isDepth = IsDepthFormat(m_Resources[handle.ID].Desc.Format);
                RHI::ResourceState target = isDepth ? RHI::ResourceState::DepthStencilWrite : RHI::ResourceState::RenderTarget;

                TransitionResource(handle, target, true);
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

        // Open the file
        std::ofstream out(outputDir / "graph.json");

        out << "{\n";

        // 1. Dump Resources
        out << "  \"resources\": [\n";
        for (size_t i = 0; i < m_Resources.size(); ++i)
        {
            // Assuming your resource struct has a 'Name' field.
            // If not, use "Resource_" + std::to_string(i)
            std::string name = m_Resources[i].Name.empty() ? "Res_" + std::to_string(i) : m_Resources[i].Name;
            out << "    { \"id\": " << i << ", \"name\": \"" << name << "\" }";
            if (i < m_Resources.size() - 1) out << ",";
            out << "\n";
        }
        out << "  ],\n";

        // 2. Dump Passes
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

    const RHI::TextureDesc& RenderGraph::GetResourceDesc(RGResourceHandle handle) const
    {
        OPAL_ASSERT("Core/RenderGraph", handle.IsValid() && handle.ID < m_Resources.size(), "Invalid Handle!");
        return m_Resources[handle.ID].Desc;
    }
}
