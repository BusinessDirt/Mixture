#pragma once

/**
 * @file IPipeline.hpp
 * @brief Interface for pipeline state objects and shaders.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/RenderStates.hpp"
#include "Mixture/Render/RHI/RenderFormats.hpp"

#include <vector>
#include <string>

namespace Mixture::RHI
{

    /**
     * @brief Defines the programmable stage of the rendering pipeline that a shader belongs to.
     */
    enum class ShaderStage : uint8_t
    {
        /**
         * @brief The vertex shader stage, processing individual vertices.
         */
        Vertex = 0,

        /**
         * @brief The fragment shader stage, processing fragments (pixels).
         */
        Fragment,

        /**
         * @brief The compute shader stage, for general-purpose computing.
         */
        Compute
    };

    /**
     * @brief Stable identity used to cache objects derived from shader code.
     *
     * StableID identifies the logical shader asset, while Version changes each
     * time that asset is reloaded. Stage distinguishes entry points within it.
     */
    struct ShaderIdentity
    {
        uint64_t StableID = 0;
        uint64_t Version = 0;
        ShaderStage Stage = ShaderStage::Vertex;

        bool operator==(const ShaderIdentity&) const = default;
        explicit operator bool() const { return StableID != 0 && Version != 0; }
    };

    /**
     * @brief Interface representing a compiled shader module.
     */
    class IShader
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IShader() = default;

        /** @brief Gets the stable, versioned identity of this shader. */
        virtual ShaderIdentity GetIdentity() const = 0;

        /**
         * @brief Retrieves the stage of this shader.
         * @return The ShaderStage of this shader.
         */
        virtual ShaderStage GetStage() const = 0;
    };


    /**
     * @brief Descriptor structure used to create a pipeline state object.
     */
    struct PipelineDesc
    {
        /**
         * @brief Pointer to the vertex shader.
         */
        IShader* VertexShader = nullptr;

        /**
         * @brief Pointer to the fragment shader.
         * Can be nullptr for depth-only passes.
         */
        IShader* FragmentShader = nullptr;

        /**
         * @brief Configuration for the rasterizer stage.
         */
        RasterizerState Rasterizer;

        /**
         * @brief Configuration for depth and stencil testing.
         */
        DepthStencilState DepthStencil;

        /**
         * @brief Configuration for color blending.
         */
        BlendState Blend;

        /**
         * @brief The primitive topology (e.g., triangles, lines).
         */
        PrimitiveTopology Topology = PrimitiveTopology::TriangleList;

        /**
         * @brief List of formats for the color attachments.
         */
        Vector<Format> ColorAttachmentFormats;

        /**
         * @brief Format of the depth attachment.
         */
        Format DepthAttachmentFormat = Format::Undefined;

        /**
         * @brief Debug name for the pipeline.
         */
        const char* DebugName = "Unnamed Pipeline";

        bool operator==(const PipelineDesc& other) const
        {
            return VertexShader == other.VertexShader &&
                   FragmentShader == other.FragmentShader &&
                   Rasterizer == other.Rasterizer &&
                   DepthStencil == other.DepthStencil &&
                   Blend == other.Blend &&
                   Topology == other.Topology &&
                   ColorAttachmentFormats == other.ColorAttachmentFormats &&
                   DepthAttachmentFormat == other.DepthAttachmentFormat;
        }
    };

    /**
     * @brief Interface representing a graphics pipeline state object.
     */
    class IPipeline
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPipeline() = default;

        /** @brief Returns whether backend pipeline construction completed successfully. */
        virtual bool IsValid() const = 0;

        // Later add methods here to get the "Layout"
        // (i.e., what descriptors does this pipeline need?)
    };
}
