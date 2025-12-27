#pragma once

/**
 * @file IGraphicsDevice.hpp
 * @brief Interface for the graphics device, acting as a factory for resources.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/IBuffer.hpp"
#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"

#include <string>

namespace Mixture::RHI
{
    /**
     * Interface for the graphics device.
     */
    class IGraphicsDevice
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~IGraphicsDevice() = default;

        // ---------------------------------------------------------------------
        // Resource Creation (Factory Methods)
        // ---------------------------------------------------------------------

        /**
         * Creates a shader from a binary blob (SPIR-V / DXIL).
         *
         * @param data Pointer to the shader bytecode.
         * @param size Size of the bytecode in bytes.
         * @param stage The shader stage.
         * @return A reference to the created shader.
         */
        virtual Ref<IShader> CreateShader(const void* data, size_t size, ShaderStage stage) = 0;

        /**
         * Creates a buffer (Vertex, Index, Uniform).
         *
         * @param desc The buffer description.
         * @return A reference to the created buffer.
         */
        virtual Ref<IBuffer> CreateBuffer(const BufferDesc& desc) = 0;

        /**
         * Creates a texture (Empty or from data).
         *
         * @param desc The texture description.
         * @return A reference to the created texture.
         */
        virtual Ref<ITexture> CreateTexture(const TextureDesc& desc) = 0;

        /**
         * Creates the PSO (Pipeline State Object).
         *
         * @param desc The pipeline description.
         * @return A reference to the created pipeline.
         */
        virtual Ref<IPipeline> CreatePipeline(const PipelineDesc& desc) = 0;

        // ---------------------------------------------------------------------
        // Frame Management
        // ---------------------------------------------------------------------

        /**
         * Waits for the GPU to finish all work (useful for resizing/shutdown).
         */
        virtual void WaitForIdle() = 0;
    };
}
