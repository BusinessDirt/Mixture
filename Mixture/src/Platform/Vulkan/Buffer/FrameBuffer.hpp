#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class ImageView;

    class FrameBuffer
    {
    public:
        MX_NON_COPIABLE(FrameBuffer);
        
        FrameBuffer(VkImageView depthAttachment, VkImage swapChainImage, VkExtent2D extent, VkFormat format, VkRenderPass renderPass = nullptr);
        ~FrameBuffer();
        
        VkFormat GetFormat() const { return m_Format; }
        const VkImage GetImage() const { return m_Image; }
        const ImageView& GetImageView() const { return *m_ImageView; }
        
    private:
        VULKAN_HANDLE(VkFramebuffer, m_FrameBuffer);
        
        const VkFormat m_Format;
        const VkImage m_Image;
        Scope<ImageView> m_ImageView;
    };
}
