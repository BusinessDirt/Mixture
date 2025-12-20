#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

namespace Mixture
{
    static bool IsDepthFormat(RHI::Format format)
    {
        switch (format)
        {
            case RHI::Format::D32_FLOAT:
            case RHI::Format::D24_UNORM_S8_UINT:
            case RHI::Format::D32_FLOAT_S8_UINT:
                return true;
            default:
                return false;
        }
    }

    void RenderGraph::Clear()
    {
        m_Passes.clear();
        m_Resources.clear();

        m_Registry.Clear();
    }

    void RenderGraph::Compile()
    {
        SortPasses();
        CalculateLifetimes();
        CalculateBarriers();

        /*// Simple sanity check loop
        for (const auto& pass : m_Passes)
        {
            OPAL_LOG_DEBUG("Core/RenderGraph", "Compiling Pass: {}", pass.Name);
        }*/
    }

    void RenderGraph::Execute(Ref<RHI::ICommandList> cmdList, RHI::IGraphicsContext* context)
    {
        // Realize Resources (Allocation Phase)
        for (const auto& node : m_Resources)
        {
            if (!node.IsImported)
            {
                auto texture = context->GetDevice()->CreateTexture(node.Desc);
                m_Registry.ImportTexture(node.Handle, texture);
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
                    cmdList->BeginRendering(renderingInfo);
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

    RGResourceHandle RenderGraph::ImportResource(const std::string& name, Ref<RHI::ITexture> resource)
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
        std::vector<std::vector<size_t>> adjacencyList(passCount);
        std::vector<int> inDegree(passCount, 0);

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

            // UPDATE: This pass is now the latest writer for its outputs
            for (auto& write : pass.Writes)
                resourceWriters[write.Handle.ID] = i;
        }

        // Kahn's Algorithm (Standard Topological Sort)
        std::vector<size_t> queue; // Passes with 0 dependencies
        for (size_t i = 0; i < passCount; ++i)
            if (inDegree[i] == 0) queue.push_back(i);

        std::vector<RGPassNode> sortedPasses;
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

        // Debug Logging
        for (const auto& node : m_Resources)
        {
            if (node.FirstPassIndex != -1)
            {
                OPAL_LOG_DEBUG("Core/RenderGraph", "Resource [{}] Life: {} -> {}",
                    node.Name, node.FirstPassIndex, node.LastPassIndex);
            }
        }
    }

    void RenderGraph::CalculateBarriers()
    {
        // Track current state of every resource (Index = ResourceID)
        // Default state is usually "Undefined" or "ShaderResource" depending on implementation
        std::vector<RHI::ResourceState> resourceStates(m_Resources.size(), RHI::ResourceState::Undefined);

        for (auto& pass : m_Passes)
        {
            pass.Barriers.clear();

            // Handle Reads (Need ShaderResource state)
            for (auto& handle : pass.Reads)
            {
                RHI::ResourceState current = resourceStates[handle.ID];
                RHI::ResourceState target = RHI::ResourceState::ShaderResource;

                if (current != target)
                {
                    // Insert Barrier: Current -> Target
                    pass.Barriers.push_back({ handle, current, target });
                    resourceStates[handle.ID] = target;
                }
            }

            // Handle Writes (Need RenderTarget or CopyDest state)
            for (auto& write : pass.Writes)
            {
                auto& handle = write.Handle;
                RHI::ResourceState current = resourceStates[handle.ID];

                // Determine target based on usage
                // Check if it's a Depth texture or standard Color
                bool isDepth = IsDepthFormat(m_Resources[handle.ID].Desc.Format);
                RHI::ResourceState target = isDepth ? RHI::ResourceState::DepthStencilWrite : RHI::ResourceState::RenderTarget;

                // Optimization: If it's already writable, we might not need a barrier
                // BUT, Write-After-Write hazards generally need a barrier or at least a memory barrier.
                // However, if the state is already RenderTarget, it implies the layout is correct.
                // Vulkan pipeline barriers also handle synchronization.
                // For safety, we emit a barrier if state differs OR if we want to be safe about WAW.
                // But simplified logic: only if state differs.
                if (current != target)
                {
                    pass.Barriers.push_back({ handle, current, target });
                    resourceStates[handle.ID] = target;
                }
            }
        }
    }
}
