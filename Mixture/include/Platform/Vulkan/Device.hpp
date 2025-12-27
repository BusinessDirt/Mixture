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
         * @brief Gets the graphics queue.
         *
         * @return vk::Queue The graphics queue.
         */
		vk::Queue GetGraphicsQueue() const { return m_GraphicsQueue; }

        /**
         * @brief Gets the transfer queue.
         *
         * @return vk::Queue The transfer queue.
         */
		vk::Queue GetTransferQueue() const { return m_TransferQueue; }

        /**
         * @brief Gets the present queue.
         *
         * @return vk::Queue The present queue.
         */
		vk::Queue GetPresentQueue() const { return m_PresentQueue; }

        /**
         * @brief Gets the compute queue.
         *
         * @return vk::Queue The compute queue.
         */
		vk::Queue GetComputeQueue() const { return m_ComputeQueue; }

        /**
         * @brief Gets the handle of the Vulkan Memory Allocator.
         *
         * @return VmaAllocator The vulkan handle of the allocator.
         */
        VmaAllocator GetAllocator() const { return m_Allocator; }

        Ref<RHI::IShader> CreateShader(const void* data, size_t size, RHI::ShaderStage stage) override;
        Ref<RHI::IBuffer> CreateBuffer(const RHI::BufferDesc& desc) override;
        Ref<RHI::ITexture> CreateTexture(const RHI::TextureDesc& desc) override;
        Ref<RHI::IPipeline> CreatePipeline(const RHI::PipelineDesc& desc) override;

		void WaitForIdle() override { m_Device.waitIdle(); }

	private:
		PhysicalDevice* m_PhysicalDevice;
		vk::Device m_Device;

		vk::Queue m_GraphicsQueue;
        vk::Queue m_TransferQueue;
		vk::Queue m_PresentQueue;
		vk::Queue m_ComputeQueue;

        VmaAllocator m_Allocator;
	};
}
