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
    class Queue;

    /**
     * @brief Wrapper around a Vulkan logical device.
     */
	class Device : public RHI::IGraphicsDevice, public std::enable_shared_from_this<Device>
	{
	public:
        /**
         * @brief Constructor.
         *
         * @param instance The vulkan instance
         * @param physicalDevice The physical device to create the logical device from.
         */
		Device(Ref<Instance> instance, Ref<PhysicalDevice> physicalDevice);
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

        /** @brief Gets the physical device retained by this logical device. */
        PhysicalDevice& GetPhysicalDevice() const { return *m_PhysicalDevice; }

        /** @brief Assigns the queue used for immediate resource uploads. */
        void SetTransferQueue(Queue& transferQueue) { m_TransferQueue = &transferQueue; }

        /** @brief Clears the non-owning upload queue before its context is destroyed. */
        void ClearTransferQueue() { m_TransferQueue = nullptr; }

        /** @brief Gets the queue used for immediate resource uploads. */
        Queue& GetTransferQueue() const;

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
		Ref<Instance> m_Instance;
		Ref<PhysicalDevice> m_PhysicalDevice;
		Queue* m_TransferQueue = nullptr;
		vk::Device m_Device = nullptr;

        VmaAllocator m_Allocator = nullptr;
	};
}
