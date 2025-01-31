#pragma once

#include "Mixture/Core/Base.hpp"

#define VULKAN_HPP_NO_TO_STRING
#include <vulkan/vulkan.hpp>

#include "Platform/Vulkan/Surface.hpp"

namespace Mixture::Vulkan
{
    struct QueueFamilyIndices;
    struct SwapChainSupportDetails;

    class PhysicalDevice
    {
    public:
        MX_NON_COPIABLE(PhysicalDevice);
        
        PhysicalDevice(const std::vector<const char*>& requiredExtensions);
        ~PhysicalDevice() = default;
        
        const VkPhysicalDeviceProperties& GetProperties() const { return m_Properties; }
        const VkPhysicalDeviceFeatures& GetFeatures() const { return m_Features; }
        const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

        SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice device = VK_NULL_HANDLE) const;
    
    private:
        int RateDeviceSuitability(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions);
        bool CheckExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions);

        QueueFamilyIndices FindQueueFamilyIndices(const VkPhysicalDevice device);
        
    private:
        VULKAN_HANDLE(VkPhysicalDevice, m_PhysicalDevice);
        
        VkPhysicalDeviceProperties m_Properties;
        VkPhysicalDeviceFeatures m_Features;

        QueueFamilyIndices m_QueueFamilyIndices;
    };
}
