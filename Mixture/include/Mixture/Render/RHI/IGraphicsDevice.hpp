#pragma once

/**
 * @file IGraphicsDevice.hpp
 * @brief Interface for the graphics device, acting as a factory for resources.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/IBuffer.hpp"
#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"
#include "Mixture/Assets/Shaders/SlangShaderReflector.hpp"

#include <string>
#include <span>
#include <optional>
#include <limits>

namespace Mixture::RHI
{
    inline std::optional<size_t> GetTextureUploadSize(const TextureDesc& desc)
    {
        const size_t stride = GetFormatStride(desc.PixelFormat);

        if (desc.Width == 0 || desc.Height == 0 || stride == 0)
            return std::nullopt;

        if (static_cast<size_t>(desc.Width) > std::numeric_limits<size_t>::max() / static_cast<size_t>(desc.Height))
            return std::nullopt;

        const size_t pixels = static_cast<size_t>(desc.Width) * static_cast<size_t>(desc.Height);

        if (pixels > std::numeric_limits<size_t>::max() / stride)
            return std::nullopt;

        return pixels * stride;
    }

    inline bool IsBufferUploadValid(const BufferDesc& desc, std::span<const std::byte> data)
    {
        return desc.Size > 0 && desc.Size <= std::numeric_limits<size_t>::max()
            && (data.empty() || data.size() == desc.Size);
    }

    inline bool IsTextureUploadValid(const TextureDesc& desc, std::span<const std::byte> data)
    {
        const auto requiredSize = GetTextureUploadSize(desc);
        return requiredSize && (data.empty() || data.size() == *requiredSize);
    }

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
         * @param identity Stable logical identity and code version.
         * @return A reference to the created shader.
         */
        virtual Ref<IShader> CreateShader(const void* data, size_t size, ShaderStage stage,
            ShaderIdentity identity, const ShaderReflectionData& reflection) = 0;

        /**
         * Creates a buffer (Vertex, Index, Uniform).
         *
         * @param desc The buffer description.
         * @param initialData Optional pointer to data to upload to the buffer.
         * @return A reference to the created buffer.
         */
        virtual Ref<IBuffer> CreateBuffer(const BufferDesc& desc,
            std::span<const std::byte> initialData = {}) = 0;

        /**
         * Creates a texture (Empty or from data).
         *
         * @param desc The texture description.
         * @param initialData Optional pointer to raw pixel data (must match format/size).
         * @return A reference to the created texture.
         */
        virtual Ref<ITexture> CreateTexture(const TextureDesc& desc,
            std::span<const std::byte> initialData = {}) = 0;

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
