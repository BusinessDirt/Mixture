#pragma once

/**
 * @file IPipeline.hpp
 * @brief Interface for pipeline state objects and shaders.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/RenderStates.hpp"
#include "Mixture/Render/RHI/RenderFormats.hpp"
#include "Mixture/Render/RHI/ShaderDefinitions.hpp"

#include "Mixture/Assets/Shaders/SlangShaderReflector.hpp"

#include <vector>
#include <string>

namespace Mixture::RHI
{

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


        virtual const ShaderReflectionData* GetShaderReflectionData() const = 0;
    };
}
