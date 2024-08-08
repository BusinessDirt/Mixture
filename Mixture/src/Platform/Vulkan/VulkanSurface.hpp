#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class Window;
    class VulkanInstance;

    struct VulkanQueueFamilyIndices
    {
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Present;
        std::optional<uint32_t> Compute;
        
        bool IsComplete();
    };

    struct VulkanSwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    class VulkanSurface
    {
    public:
        MX_NON_COPIABLE(VulkanSurface);
        
        VulkanSurface(const Window& window, const VulkanInstance& instance);
        ~VulkanSurface();
        
        VulkanQueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice device) const;
        VulkanSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
        
    private:
        VULKAN_HANDLE(VkSurfaceKHR, m_Surface);
        const VulkanInstance& m_Instance;
    };
}
