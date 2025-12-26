#pragma once

/**
 * @file RenderGraphBuilder.hpp
 * @brief Helper for defining pass resource usage.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"
#include "Mixture/Render/RHI/IPipeline.hpp"

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
         * @brief Declares that this pass writes to a resource with specific attachment settings.
         *
         * @param info The attachment info (handle, load/store ops, clear color).
         * @return RGResourceHandle The handle from the info.
         */
        RGResourceHandle Write(const RGAttachmentInfo& info);

        /**
         * @brief Creates a new internal transient resource for this pass.
         *
         * @param name The name of the resource.
         * @param desc The description of the texture to create.
         * @return RGResourceHandle A handle to the created resource.
         */
        RGResourceHandle CreateTexture(const std::string& name, const RHI::TextureDesc& desc);

        /**
         * @brief Creates a new internal transient buffer for this pass.
         *
         * @param name The name of the resource.
         * @param desc The description of the buffer to create.
         * @return RGResourceHandle A handle to the created resource.
         */
        RGResourceHandle CreateBuffer(const std::string& name, const RHI::BufferDesc& desc);

        /**
         * @brief Loads a shader (or retrieves it from cache) using the AssetSystem.
         *
         * @param path Relative path to the shader source file (e.g. "Assets/Shader/Triangle.hlsl")
         * @param stage The shader stage (Vertex, Fragment, etc.)
         * @return RHI::IShader* Pointer to the loaded shader.
         */
        RHI::IShader* LoadShader(const std::string& path, RHI::ShaderStage stage);

        /**
         * @brief Creates (or retrieves from cache) a pipeline state object.
         * Automatically fills in the Color/Depth attachment formats based on the pass Writes.
         *
         * @param desc The pipeline description (Shaders, State). Formats can be left empty.
         * @return RHI::IPipeline* The created pipeline.
         */
        RHI::IPipeline* CreatePipeline(RHI::PipelineDesc& desc);

    private:
        RenderGraph& m_Graph;
        RGPassNode& m_PassNode;

        Vector<RHI::Format> m_CurrentColorFormats;
        RHI::Format m_CurrentDepthFormat = RHI::Format::Undefined;
    };
}
