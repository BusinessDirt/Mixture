#include "mxpch.hpp"
#include "Platform/Vulkan/Device.hpp"

#include "Platform/Vulkan/Resources/Texture.hpp"
#include "Platform/Vulkan/Resources/Buffer.hpp"

#include "Platform/Vulkan/Pipeline/Pipeline.hpp"
#include "Platform/Vulkan/Pipeline/Shader.hpp"

#include <vector>
#include <set>

namespace Mixture::Vulkan
{
	Device::Device(Instance& instance, PhysicalDevice& physicalDevice)
		: m_PhysicalDevice(&physicalDevice)
	{
		auto indices = m_PhysicalDevice->GetQueueFamilies();

		float queuePriority = 1.0f;
		vk::DeviceQueueCreateInfo queueCreateInfo(
			vk::DeviceQueueCreateFlags(),
			indices.Graphics.value(),
			1, &queuePriority
		);

		Vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

        // Check for Portability Subset (MacOS / MoltenVK)
        std::vector<vk::ExtensionProperties> availableExtensions =
            m_PhysicalDevice->GetHandle().enumerateDeviceExtensionProperties();

        for (const auto& ext : availableExtensions)
        {
            if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0)
            {
                deviceExtensions.push_back("VK_KHR_portability_subset");
                break;
            }
        }

        vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

        vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        vk::DeviceCreateInfo createInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.pNext = &dynamicRenderingFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.enabledLayerCount = 0;

        try
        {
            m_Device = m_PhysicalDevice->GetHandle().createDevice(createInfo);
        }
        catch (vk::SystemError& err)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to create logical device!");
            exit(-1);
        }

        m_GraphicsQueue = m_Device.getQueue(indices.Graphics.value(), 0);

        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorInfo.physicalDevice = m_PhysicalDevice->GetHandle();
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = instance.GetHandle();
        allocatorInfo.pVulkanFunctions = &vulkanFunctions;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to create VMA Allocator!");
            exit(-1);
        }

        OPAL_INFO("Core/Vulkan", "VMA Initialized.");
	}

	Device::~Device()
	{
        m_Device.waitIdle();
        vmaDestroyAllocator(m_Allocator);
        m_Device.destroy();
	}

    Ref<RHI::IShader> Device::CreateShader(const void* data, size_t size, RHI::ShaderStage stage)
    {
        return CreateRef<Shader>(data, size, stage);
    }

    Ref<RHI::IBuffer> Device::CreateBuffer(const RHI::BufferDesc& desc)
    {
        return CreateRef<Buffer>(desc);
    }

    Ref<RHI::ITexture> Device::CreateTexture(const RHI::TextureDesc& desc)
    {
        return CreateRef<Texture>(desc);
    }

    Ref<RHI::IPipeline> Device::CreatePipeline(const RHI::PipelineDesc& desc)
    {
        return CreateRef<Pipeline>(desc);
    }
}
