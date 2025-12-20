#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraphBuilder.hpp"

#include "Mixture/Render/Graph/RenderGraph.hpp"

namespace Mixture
{
    RenderGraphBuilder::RenderGraphBuilder(RenderGraph& graph, RGPassNode& passNode)
        : m_Graph(graph), m_PassNode(passNode)
    {
    }

    RGResourceHandle RenderGraphBuilder::Read(RGResourceHandle handle)
    {
        if (!handle.IsValid())
        {
            OPAL_WARN("Core/RenderGraph", "RenderGraphBuilder::Read - RGResourceHandle is invalid");
            return handle;
        }

        // Record that this pass READS this resource
        m_PassNode.Reads.push_back(handle);
        return handle;
    }

    RGResourceHandle RenderGraphBuilder::Write(RGResourceHandle handle)
    {
        if (!handle.IsValid())
        {
            OPAL_WARN("Core/RenderGraph", "RenderGraphBuilder::Write - RGResourceHandle is invalid");
            return handle;
        }

        // Record that this pass WRITES to this resource
        m_PassNode.Writes.push_back(handle);

        // Future Proofing: If implementing resource versioning (renaming),
        // this is where a NEW handle ID would be returned.
        // For now, we return the same one.
        return handle;
    }

    RGResourceHandle RenderGraphBuilder::Create(const std::string& name, const RHI::TextureDesc& desc)
    {
        // Delegate the actual allocation logic to the main graph
        return m_Graph.CreateResource(name, desc);
    }
}
