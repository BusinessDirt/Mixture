#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>


namespace Mixture::Vulkan
{
    struct QueueFamilyIndices;
    struct SwapChainSupportDetails;

    class PhysicalDevice
    {
    public:
        MX_NON_COPIABLE(PhysicalDevice);
        
        PhysicalDevice();
        ~PhysicalDevice() = default;
        
        QueueFamilyIndices FindQueueFamilyIndices() const;
        SwapChainSupportDetails QuerySwapChainSupport() const;
    
    private:
        bool IsDeviceSuitable(VkPhysicalDevice device);
        
    private:
        VULKAN_HANDLE(VkPhysicalDevice, m_PhysicalDevice);
    };
}