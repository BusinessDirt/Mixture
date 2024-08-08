#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanPhysicalDevice;
    class VulkanDevice;
    class VulkanSurface;

    class VulkanSwapChain
    {
    public:
        MX_NON_COPIABLE( VulkanSwapChain );
        
        VulkanSwapChain(const VulkanPhysicalDevice& physicalDevice, const VulkanDevice& device, const VulkanSurface& surface);
        ~VulkanSwapChain();
        
    private:
        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        
    private:
        VULKAN_HANDLE(VkSwapchainKHR, m_SwapChain);
        
        std::vector<VkImage> m_Images;
        VkFormat m_Format;
        VkExtent2D m_Extent;
        
        const VulkanDevice& m_Device;
    };
}
