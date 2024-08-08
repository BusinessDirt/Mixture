#pragma once

#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/VulkanQueueFamilies.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanQueueFamilies;
    class VulkanManager;

    class VulkanDevice
    {
    public:
        MX_NON_COPIABLE(VulkanDevice);
        
        VulkanDevice(VkPhysicalDevice physicalDevice, const VulkanManager& manager);
        ~VulkanDevice();
        
    private:
        VULKAN_HANDLE(VkDevice, m_Device);
        
        QueueFamilyIndices m_QueueFamilies{};
        VkQueue m_GraphicsQueue{VK_NULL_HANDLE};
    };
}
