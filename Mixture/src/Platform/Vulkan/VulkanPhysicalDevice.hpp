#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanInstance;
    class VulkanSurface;

    struct VulkanQueueFamilyIndices;
    struct VulkanSwapChainSupportDetails;

    class VulkanPhysicalDevice
    {
    public:
        MX_NON_COPIABLE(VulkanPhysicalDevice);
        
        VulkanPhysicalDevice();
        ~VulkanPhysicalDevice() = default;
        
        VulkanQueueFamilyIndices FindQueueFamilyIndices() const;
        VulkanSwapChainSupportDetails QuerySwapChainSupport() const;
    
    private:
        bool IsDeviceSuitable(VkPhysicalDevice device);
        
    private:
        VULKAN_HANDLE(VkPhysicalDevice, m_PhysicalDevice);
    };
}
