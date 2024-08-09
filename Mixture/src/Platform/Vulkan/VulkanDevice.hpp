#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanManager;
    class VulkanPhysicalDevice;

    class VulkanDevice
    {
    public:
        MX_NON_COPIABLE(VulkanDevice);
        
        VulkanDevice(const VulkanManager& manager);
        ~VulkanDevice();
        
    private:
        VULKAN_HANDLE(VkDevice, m_Device);
        
        uint32_t m_GraphicsQueueIndex;
        VkQueue m_GraphicsQueue{VK_NULL_HANDLE};
        
        uint32_t m_PresentQueueIndex;
        VkQueue m_PresentQueue{VK_NULL_HANDLE};
    };
}
