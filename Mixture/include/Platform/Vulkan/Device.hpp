#pragma once

/**
 * @file Device.hpp
 * @brief Vulkan logical device wrapper.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

#include <vma/vk_mem_alloc.h>

namespace Mixture::Vulkan
{
    /**
     * @brief Wrapper around a Vulkan logical device.
     */
	class Device : public RHI::IGraphicsDevice
	{
	public:
        /**
         * @brief Constructor.
         *
         * @param instance The vulkan instance
         * @param physicalDevice The physical device to create the logical device from.
         */
		Device(Instance& instance, PhysicalDevice& physicalDevice);
		~Device();

        /**
         * @brief Gets the Vulkan device handle.
         *
         * @return vk::Device The raw handle.
         */
		vk::Device GetHandle() const { return m_Device; }

        /**
         * @brief Gets the handle of the Vulkan Memory Allocator.
         *
         * @return VmaAllocator The vulkan handle of the allocator.
         */
        VmaAllocator GetAllocator() const { return m_Allocator; }

        /**
         * @brief Creates a Vulkan shader module.
         * 
         * @param data Pointer to the shader bytecode.
         * @param size Size of the bytecode in bytes.
         * @param stage The shader stage.
         * @return Ref<RHI::IShader> The created shader.
         */
        Ref<RHI::IShader> CreateShader(const void* data, size_t size, RHI::ShaderStage stage) override;

        /**
         * @brief Creates a Vulkan buffer.
         * 
         * @param desc The buffer description.
         * @param initialData Optional pointer to data to upload.
         * @return Ref<RHI::IBuffer> The created buffer.
         */
        Ref<RHI::IBuffer> CreateBuffer(const RHI::BufferDesc& desc, const void* initialData = nullptr) override;

        /**
         * @brief Creates a Vulkan texture.
         * 
         * @param desc The texture description.
         * @param initialData Optional pointer to raw pixel data.
         * @return Ref<RHI::ITexture> The created texture.
         */
        Ref<RHI::ITexture> CreateTexture(const RHI::TextureDesc& desc, const void* initialData = nullptr) override;

        /**
         * @brief Creates a Vulkan pipeline.
         * 
         * @param desc The pipeline description.
         * @return Ref<RHI::IPipeline> The created pipeline.
         */
        Ref<RHI::IPipeline> CreatePipeline(const RHI::PipelineDesc& desc) override;

		void WaitForIdle() override { m_Device.waitIdle(); }

	private:
		PhysicalDevice* m_PhysicalDevice;
		vk::Device m_Device;

        VmaAllocator m_Allocator;
	};
}
