#include "mxpch.hpp"
#include "VulkanDevice.hpp"

#include <set>

#include "Platform/Vulkan/VulkanContext.hpp"

#include "Platform/Vulkan/VulkanManager.hpp"

namespace Mixture
{
    VulkanDevice::VulkanDevice(const VulkanManager& manager)
    {
        manager.CheckDeviceExtensionSupport(VulkanContext::Get().PhysicalDevice->GetHandle());
        
        VulkanQueueFamilyIndices indices = VulkanContext::Get().PhysicalDevice->FindQueueFamilyIndices();
        m_GraphicsQueueIndex = indices.Graphics.value();
        m_PresentQueueIndex = indices.Present.value();
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { m_GraphicsQueueIndex, m_PresentQueueIndex };
        
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        
        VkPhysicalDeviceFeatures deviceFeatures{};
        
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(manager.GetDeviceExtensions().size());
        createInfo.ppEnabledExtensionNames = manager.GetDeviceExtensions().data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(manager.GetLayers().size());
        createInfo.ppEnabledLayerNames = manager.GetLayers().data();
        
        MX_VK_ASSERT(vkCreateDevice(VulkanContext::Get().PhysicalDevice->GetHandle(), &createInfo, nullptr, &m_Device),
                     "Failed to create VkDevice");
        
        vkGetDeviceQueue(m_Device, m_GraphicsQueueIndex, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_PresentQueueIndex, 0, &m_PresentQueue);
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
