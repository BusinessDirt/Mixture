#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class SwapChain
    {
    public:
        MX_NON_COPIABLE(SwapChain);
        
        SwapChain();
        ~SwapChain();
        
    public:
        const VkFormat& GetFormat() const { return m_Format; }
        const VkExtent2D& GetExtent() const { return m_Extent; }
        
    private:
        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        
    private:
        VULKAN_HANDLE(VkSwapchainKHR, m_SwapChain);
        
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        VkFormat m_Format;
        VkExtent2D m_Extent;
    };
}
