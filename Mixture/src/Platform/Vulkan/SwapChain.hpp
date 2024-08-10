#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
class RenderPass;
    class FrameBuffer;

    class SwapChain
    {
    public:
        MX_NON_COPIABLE(SwapChain);
        
        SwapChain();
        ~SwapChain();
        
    public:
        const VkFormat& GetFormat() const { return m_Format; }
        const VkExtent2D& GetExtent() const { return m_Extent; }
        const RenderPass& GetRenderPass() const { return *m_RenderPass;}
        
    private:
        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        
        void CreateImageViews();
        void CreateFramebuffers();
        
    private:
        VULKAN_HANDLE(VkSwapchainKHR, m_SwapChain);
        
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        Scope<RenderPass> m_RenderPass;
        std::vector<FrameBuffer> m_FrameBuffers;
        VkFormat m_Format;
        VkExtent2D m_Extent;
    };
}
