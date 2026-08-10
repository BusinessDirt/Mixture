#include "mxpch.hpp"
#include "Platform/Vulkan/Device.hpp"

#include "Platform/Vulkan/Resources/Texture.hpp"
#include "Platform/Vulkan/Resources/Buffer.hpp"

#include "Platform/Vulkan/Pipeline/Pipeline.hpp"
#include "Platform/Vulkan/Pipeline/Shader.hpp"
#include "Platform/Vulkan/Queue.hpp"

#include <vector>
#include <set>
#include <stdexcept>

namespace Mixture::Vulkan
{
	Device::Device(Ref<Instance> instance, Ref<PhysicalDevice> physicalDevice)
		: m_Instance(std::move(instance)), m_PhysicalDevice(std::move(physicalDevice))
	{
		if (!m_Instance || !m_PhysicalDevice)
            throw std::invalid_argument("Device requires explicit instance and physical-device ownership");

		auto indices = m_PhysicalDevice->GetQueueFamilies();

		float queuePriority = 1.0f;
		const auto queueFamilies = CollectQueueFamilyIndices(indices);
		Vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(queueFamilies.size());
		for (const uint32_t family : queueFamilies)
			queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), family, 1, &queuePriority);

		Vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

        // Check for Portability Subset (MacOS / MoltenVK)
        std::vector<vk::ExtensionProperties> availableExtensions =
            m_PhysicalDevice->GetHandle().enumerateDeviceExtensionProperties();
        if (!PhysicalDevice::HasRequiredExtensions(availableExtensions))
            throw std::runtime_error("Selected Vulkan device does not support VK_KHR_swapchain");

        for (const auto& ext : availableExtensions)
        {
            if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0)
            {
                deviceExtensions.push_back("VK_KHR_portability_subset");
                break;
            }
        }

        vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures;
        bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

        vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
        dynamicRenderingFeatures.pNext = &bufferDeviceAddressFeatures;

        vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        vk::PhysicalDeviceBufferDeviceAddressFeatures availableBufferDeviceAddress;
        vk::PhysicalDeviceDynamicRenderingFeatures availableDynamicRendering;
        availableDynamicRendering.pNext = &availableBufferDeviceAddress;
        vk::PhysicalDeviceFeatures2 availableFeatures;
        availableFeatures.pNext = &availableDynamicRendering;
        m_PhysicalDevice->GetHandle().getFeatures2(&availableFeatures);
        if (!PhysicalDevice::HasRequiredFeatures(availableFeatures.features.samplerAnisotropy,
            availableDynamicRendering.dynamicRendering, availableBufferDeviceAddress.bufferDeviceAddress))
            throw std::runtime_error("Selected Vulkan device is missing required anisotropy, dynamic-rendering, or buffer-address features");

        vk::DeviceCreateInfo createInfo;
        createInfo.setQueueCreateInfos(queueCreateInfos);
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.pNext = &dynamicRenderingFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        try
        {
            m_Device = m_PhysicalDevice->GetHandle().createDevice(createInfo);
        }
        catch (vk::SystemError& err)
        {
            throw std::runtime_error(std::string("Failed to create Vulkan logical device: ") + err.what());
        }

        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorInfo.physicalDevice = m_PhysicalDevice->GetHandle();
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance->GetHandle();
        allocatorInfo.pVulkanFunctions = &vulkanFunctions;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
        {
            m_Device.destroy();
            m_Device = nullptr;
            throw std::runtime_error("Failed to create the Vulkan memory allocator");
        }

        OPAL_INFO("Core/Vulkan", "VMA Initialized.");
	}

	Device::~Device()
	{
		if (!m_Device) return;

        m_Device.waitIdle();
        if (m_Allocator) vmaDestroyAllocator(m_Allocator);
        m_Device.destroy();
	}

    Queue& Device::GetTransferQueue() const
    {
        OPAL_ASSERT("Core/Vulkan", m_TransferQueue, "Transfer queue has not been assigned to the device");
        return *m_TransferQueue;
    }

    void Device::Submit(vk::Queue queue, const vk::SubmitInfo& submitInfo, vk::Fence fence)
    {
        std::lock_guard<std::mutex> lock(m_QueueSubmitMutex);
        queue.submit(submitInfo, fence);
    }

    Ref<RHI::IShader> Device::CreateShader(const void* data, size_t size, RHI::ShaderStage stage,
        RHI::ShaderIdentity identity)
    {
        return CreateRef<Shader>(shared_from_this(), data, size, stage, identity);
    }

    Ref<RHI::IBuffer> Device::CreateBuffer(const RHI::BufferDesc& desc, const void* initialData)
    {
        return CreateRef<Buffer>(shared_from_this(), desc, initialData);
    }

    Ref<RHI::ITexture> Device::CreateTexture(const RHI::TextureDesc& desc, const void* initialData)
    {
        return CreateRef<Texture>(shared_from_this(), desc, initialData);
    }

    Ref<RHI::IPipeline> Device::CreatePipeline(const RHI::PipelineDesc& desc)
    {
        return CreateRef<Pipeline>(shared_from_this(), desc);
    }
}
