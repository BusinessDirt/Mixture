#include "mxpch.hpp"
#include "FrameBuffer.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    FrameBuffer::FrameBuffer(VkImageView attachment, VkExtent2D extent, VkRenderPass renderPass)
    {
        VkImageView attachments[] = { attachment };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass ? renderPass : Context::Get().SwapChain->GetRenderPass().GetHandle();
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        createInfo.width = extent.width;
        createInfo.height = extent.height;
        createInfo.layers = 1;

        MX_VK_ASSERT(vkCreateFramebuffer(Context::Get().Device->GetHandle(), &createInfo, nullptr, &m_FrameBuffer), "Failed to create VkFramebuffer");
    }

    FrameBuffer::~FrameBuffer()
    {
        if (m_FrameBuffer)
        {
            vkDestroyFramebuffer(Context::Get().Device->GetHandle(), m_FrameBuffer, nullptr);
            m_FrameBuffer = nullptr;
        }
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& other)
        : m_FrameBuffer(other.m_FrameBuffer)
    {
        other.m_FrameBuffer = VK_NULL_HANDLE;
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other)
    {
        if (this != &other)
        {
            this->~FrameBuffer();
            m_FrameBuffer = other.m_FrameBuffer;
            other.m_FrameBuffer = VK_NULL_HANDLE;
        }
        return *this;
    }
}
