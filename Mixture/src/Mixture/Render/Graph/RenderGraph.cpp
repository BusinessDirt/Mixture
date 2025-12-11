#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraph.hpp"

namespace Mixture
{
    void RenderGraph::Compile()
    {
        // Re-order passes so dependencies are met
        SortPasses();

        // Insert barriers between passes
        CalculateBarriers();
        
        // Simple sanity check loop
        for (const auto& pass : m_Passes) 
        {
            OPAL_INFO("Core_RenderGraph", "Compiling Pass: {}", pass.Name);
        }
    }

    void RenderGraph::Execute(RHI::ICommandList* cmdList)
    {
        // ... (Normally allocate the resources into the registry using Device here) ...

        for (const auto& pass : m_Passes) 
        {
            // Submit Barriers before the pass starts
            for (const auto& barrier : pass.Barriers) 
            {
                RHI::ITexture* tex = m_Registry.GetTexture(barrier.Resource);
                if (tex) cmdList->PipelineBarrier(
                    tex, 
                    barrier.Before, 
                    barrier.After
                );
            }

            if (pass.Execute) 
            {
                pass.Execute(m_Registry, cmdList); 
            }
        }
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
            for (auto& handle : pass.Writes) 
                resourceWriters[handle.ID] = i;
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
            OPAL_CRITICAL("Core_RenderGraph", "Cyclic pass dependency detected.");
            return;
        }
        
        m_Passes = std::move(sortedPasses);
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
            for (auto& handle : pass.Writes) 
            {
                RHI::ResourceState current = resourceStates[handle.ID];
                
                // Determine target based on usage (TODO: Simplification: assume Color Attachment for now)
                // TODO: Check if it's a Depth texture or standard Color
                RHI::ResourceState target = RHI::ResourceState::RenderTarget;
                
                // TODO: Optimization: If it's already writable, maybe we don't need a barrier?
                // (Depends on if a Memory Barrier for Write-after-Write hazard is required)
                if (current != target)
                {
                    pass.Barriers.push_back({ handle, current, target });
                    resourceStates[handle.ID] = target;
                }
            }
        }
    }
}
