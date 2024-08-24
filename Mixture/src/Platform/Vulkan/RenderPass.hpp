#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class FrameBuffer;

    class RenderPass
    {
    public:
        MX_NON_COPIABLE(RenderPass);
        
        RenderPass(VkFormat format, bool useDepth = true, VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        ~RenderPass();
        
    private:
        VULKAN_HANDLE(VkRenderPass, m_RenderPass);
    };
}
