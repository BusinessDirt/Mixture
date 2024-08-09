#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class Window;
}

namespace Mixture::Vulkan
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Present;
        std::optional<uint32_t> Compute;
        
        bool IsComplete();
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    class Surface
    {
    public:
        MX_NON_COPIABLE(Surface);
        
        Surface(const Window& window);
        ~Surface();
        
        QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice device) const;
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;
        
    private:
        VULKAN_HANDLE(VkSurfaceKHR, m_Surface);
    };
}
