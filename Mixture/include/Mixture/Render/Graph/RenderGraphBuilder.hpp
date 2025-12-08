#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"

namespace Mixture 
{

    class RenderGraph; // Forward decl

    class RenderGraphBuilder 
    {
    public:
        RenderGraphBuilder(RenderGraph& graph, RGPassNode& passNode);

        // declare that this pass reads from a resource
        RGResourceHandle Read(RGResourceHandle handle);

        // declare that this pass writes to a resource
        // (This might rename the handle if implementing versioning later)
        RGResourceHandle Write(RGResourceHandle handle);

        // Create a new internal transient resource
        RGResourceHandle Create(const std::string& name, const RHI::TextureDesc& desc);

    private:
        RenderGraph& m_Graph;
        RGPassNode& m_PassNode;
    };
}
