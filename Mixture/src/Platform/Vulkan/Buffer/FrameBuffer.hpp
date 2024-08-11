#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class FrameBuffer
    {
    public:
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        
        FrameBuffer(FrameBuffer&& other);
        FrameBuffer& operator=(FrameBuffer&& other);
        
        FrameBuffer(const std::vector<VkImageView>& attachments, VkExtent2D extent, VkRenderPass renderPass = nullptr);
        ~FrameBuffer();
        
    private:
        VULKAN_HANDLE(VkFramebuffer, m_FrameBuffer);
    };
}
