#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"

namespace Mixture 
{

    class RenderGraph; // Forward decl

    /**
     * @brief Helper class to define resource usage (Reads/Writes) for a specific pass during setup.
     */
    class RenderGraphBuilder 
    {
    public:
        RenderGraphBuilder(RenderGraph& graph, RGPassNode& passNode);

        /**
         * @brief Declares that this pass reads from a resource.
         * 
         * @param handle The handle of the resource to read.
         * @return RGResourceHandle The same handle (passed through).
         */
        RGResourceHandle Read(RGResourceHandle handle);

        /**
         * @brief Declares that this pass writes to a resource.
         * 
         * @param handle The handle of the resource to write.
         * @return RGResourceHandle The same handle, or a new versioned handle if implementing versioning.
         */
        RGResourceHandle Write(RGResourceHandle handle);

        /**
         * @brief Creates a new internal transient resource for this pass.
         * 
         * @param name The name of the resource.
         * @param desc The description of the texture to create.
         * @return RGResourceHandle A handle to the created resource.
         */
        RGResourceHandle Create(const std::string& name, const RHI::TextureDesc& desc);

    private:
        RenderGraph& m_Graph;
        RGPassNode& m_PassNode;
    };
}
