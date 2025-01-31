#include "mxpch.hpp"
#include "Device.hpp"

#include <set>

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    Device::Device(const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions)
    {
        QueueFamilyIndices indices = Context::Get().GetPhysicalDevice().GetQueueFamilyIndices();
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
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
        createInfo.ppEnabledLayerNames = requiredLayers.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        
        MX_VK_ASSERT(vkCreateDevice(Context::Get().GetPhysicalDevice().GetHandle(), &createInfo, nullptr, &m_Device),
                     "Failed to create VkDevice");
        
        vkGetDeviceQueue(m_Device, m_GraphicsQueueIndex, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_PresentQueueIndex, 0, &m_PresentQueue);
    }

    Device::~Device()
    {
        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = nullptr;
        }
    }

    VkFormat Device::FindSupportedFormat( const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(Context::Get().GetPhysicalDevice().GetHandle(), format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        MX_CORE_ERROR("Failed to find supported format");
        return VK_FORMAT_UNDEFINED;
    }

    uint32_t Device::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(Context::Get().GetPhysicalDevice().GetHandle(), &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) &&(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        MX_CORE_ERROR("Failed to find suitable memory type");
        return 0;
    }

    void Device::CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties,
        VkImage& image, VkDeviceMemory& imageMemory)
    {
        MX_VK_ASSERT(vkCreateImage(m_Device, &imageInfo, nullptr, &image), "Failed to create VkImage");

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        MX_VK_ASSERT(vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory), "Failed to allocate VkImageMemory");
        MX_VK_ASSERT(vkBindImageMemory(m_Device, image, imageMemory, 0), "Failed to bind VkImageMemory");
    }
}
