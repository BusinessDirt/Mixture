#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanInstance;
    class VulkanSurface;
    struct VulkanQueueFamilyIndices;

    class VulkanPhysicalDevice
    {
    public:
        MX_NON_COPIABLE(VulkanPhysicalDevice);
        
        VulkanPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface);
        ~VulkanPhysicalDevice() = default;
        
        VulkanQueueFamilyIndices FindQueueFamilyIndices() const;
    
    private:
        bool IsDeviceSuitable(VkPhysicalDevice device);
        
    private:
        VULKAN_HANDLE(VkPhysicalDevice, m_PhysicalDevice);
        
        const VulkanInstance& m_Instance;
        const VulkanSurface& m_Surface;
    };
}
