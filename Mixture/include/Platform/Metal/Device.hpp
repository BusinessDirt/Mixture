#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Device.hpp
 * @brief Metal device wrapper.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

namespace Mixture::Metal
{
    /**
     * @brief Wrapper around a Metal device.
     */
	class Device : public RHI::IGraphicsDevice, public std::enable_shared_from_this<Device>
	{
	public:
        /**
         * @brief Constructor.
         */
		Device();
		~Device();

        /**
         * @brief Gets the Metal device handle.
         *
         * @return MTL::Device* The raw handle.
         */
		MTL::Device* GetHandle() const { return m_Device; }

        /**
         * @brief Gets the Metal command queue handle.
         *
         * @return MTL::CommandQueue* The command queue handle.
         */
		MTL::CommandQueue* GetCommandQueue() const { return m_CommandQueue; }

        /**
         * @brief Creates a Metal shader module.
         *
         * @param data Pointer to the shader bytecode.
         * @param size Size of the bytecode in bytes.
         * @param stage The shader stage.
         * @param identity Stable logical identity and code version.
         * @return Ref<RHI::IShader> The created shader.
         */
        Ref<RHI::IShader> CreateShader(const void* data, size_t size, RHI::ShaderStage stage,
            RHI::ShaderIdentity identity) override;

        /**
         * @brief Creates a Metal buffer.
         *
         * @param desc The buffer description.
         * @param initialData Optional pointer to data to upload.
         * @return Ref<RHI::IBuffer> The created buffer.
         */
        Ref<RHI::IBuffer> CreateBuffer(const RHI::BufferDesc& desc,
            std::span<const std::byte> initialData = {}) override;

        /**
         * @brief Creates a Metal texture.
         *
         * @param desc The texture description.
         * @param initialData Optional pointer to raw pixel data.
         * @return Ref<RHI::ITexture> The created texture.
         */
        Ref<RHI::ITexture> CreateTexture(const RHI::TextureDesc& desc,
            std::span<const std::byte> initialData = {}) override;

        /**
         * @brief Creates a Metal pipeline.
         *
         * @param desc The pipeline description.
         * @return Ref<RHI::IPipeline> The created pipeline.
         */
        Ref<RHI::IPipeline> CreatePipeline(const RHI::PipelineDesc& desc) override;

		void WaitForIdle() override {}

	private:
		MTL::Device* m_Device = nullptr;
		MTL::CommandQueue* m_CommandQueue = nullptr;
	};
}

#endif
