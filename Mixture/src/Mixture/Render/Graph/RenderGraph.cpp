#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraph.hpp"

namespace Mixture
{
    void RenderGraph::Compile()
    {
        // TODO: Topological Sort & Barrier Calculation
        // For now, we just leave the passes in the order they were added (Submission Order).
        
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
}
