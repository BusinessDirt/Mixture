#include "mxpch.hpp"
#include "VulkanDevice.hpp"

#include "Platform/Vulkan/VulkanManager.hpp"

namespace Mixture
{
    VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice, const VulkanManager& manager)
    {
        m_QueueFamilies = VulkanQueueFamilies::Find(physicalDevice);
        
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_QueueFamilies.Graphics.value();
        queueCreateInfo.queueCount = 1;
        
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        
        VkPhysicalDeviceFeatures deviceFeatures{};
        
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(manager.GetDeviceExtensions().size());
        createInfo.ppEnabledExtensionNames = manager.GetDeviceExtensions().data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(manager.GetLayers().size());
        createInfo.ppEnabledLayerNames = manager.GetLayers().data();
        
        MX_VK_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_Device),
                     "Failed to create logical device!");
        
        vkGetDeviceQueue(m_Device, m_QueueFamilies.Graphics.value(), 0, &m_GraphicsQueue);
    }

    VulkanDevice::~VulkanDevice()
    {
        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = nullptr;
        }
    }
}
