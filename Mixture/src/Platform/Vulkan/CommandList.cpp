#include "mxpch.hpp"
#include "Platform/Vulkan/CommandList.hpp"

namespace Mixture::Vulkan
{
    namespace
    {
        namespace Utils
        {
            static vk::AttachmentLoadOp ConvertLoadOp(RHI::LoadOp op)
            {
                switch (op)
                {
                    case RHI::LoadOp::Clear: return vk::AttachmentLoadOp::eClear;
                    case RHI::LoadOp::Load:  return vk::AttachmentLoadOp::eLoad;
                    case RHI::LoadOp::DontCare: return vk::AttachmentLoadOp::eDontCare;
                    case RHI::LoadOp::None: return vk::AttachmentLoadOp::eNone;
                    default: return vk::AttachmentLoadOp::eDontCare;
                }
            }

            static vk::AttachmentStoreOp ConvertStoreOp(RHI::StoreOp op)
            {
                switch (op)
                {
                    case RHI::StoreOp::Store: return vk::AttachmentStoreOp::eStore;
                    case RHI::StoreOp::DontCare: return vk::AttachmentStoreOp::eDontCare;
                    case RHI::StoreOp::None: return vk::AttachmentStoreOp::eNone;
                    default: return vk::AttachmentStoreOp::eStore;
                }
            }

            void InsertImageBarrier(vk::CommandBuffer cmdbuffer, vk::Image image,
                vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
                vk::AccessFlags srcAccess, vk::AccessFlags dstAccess)
            {
                vk::ImageMemoryBarrier barrier;
                barrier.oldLayout = oldLayout;
                barrier.newLayout = newLayout;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = image;
                barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                barrier.srcAccessMask = srcAccess;
                barrier.dstAccessMask = dstAccess;

                cmdbuffer.pipelineBarrier(
                    srcStage, dstStage,
                    vk::DependencyFlags(),
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
                );
            }
        }
    }

    void CommandList::Begin()
    {
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit; // Reset every frame
        m_CommandBuffer.begin(beginInfo);

        Utils::InsertImageBarrier(
            m_CommandBuffer, m_SwapchainImage,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::AccessFlags(),
            vk::AccessFlagBits::eColorAttachmentWrite
        );
    }

    void CommandList::End()
    {
        Utils::InsertImageBarrier(
            m_CommandBuffer, m_SwapchainImage,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::AccessFlags() // Presentation reads implicitly
        );

        m_CommandBuffer.end();
    }

    void CommandList::BeginRendering(const RHI::RenderingInfo& info)
    {
        /*
        Vector<vk::RenderingAttachmentInfo> vkColorAttachments;
        vkColorAttachments.reserve(info.ColorAttachments.size());

        for (const auto& attachment : info.ColorAttachments) {
            if (!attachment.Image) continue;

            // Cast abstract ITexture to concrete VulkanTexture to get the view
            // NOTE: Ensure your Vulkan::Texture class has GetImageView()
            auto* vulkanTexture = static_cast<Vulkan::Texture*>(attachment.Image);

            vk::RenderingAttachmentInfo vkInfo;
            vkInfo.imageView = vulkanTexture->GetImageView();
            vkInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal; // Or infer from texture usage

            // Operations
            vkInfo.loadOp = Utils::ConvertLoadOp(attachment.LoadOp);
            vkInfo.storeOp = Utils::ConvertStoreOp(attachment.StoreOp);

            // Clear Color (R, G, B, A)
            vkInfo.clearValue.color = std::array<float, 4>{
                attachment.ClearColor[0],
                attachment.ClearColor[1],
                attachment.ClearColor[2],
                attachment.ClearColor[3]
            };

            vkColorAttachments.push_back(vkInfo);
        }

        vk::RenderingAttachmentInfo vkDepthAttachment;
        bool hasDepth = (info.DepthAttachment != nullptr && info.DepthAttachment->Image != nullptr);

        if (hasDepth)
        {
            const auto& attachment = *info.DepthAttachment;
            auto* vulkanTexture = static_cast<Vulkan::Texture*>(attachment.Image);

            vkDepthAttachment.imageView = vulkanTexture->GetImageView();
            vkDepthAttachment.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vkDepthAttachment.loadOp = Utils::ConvertLoadOp(attachment.LoadOp);
            vkDepthAttachment.storeOp = Utils::ConvertStoreOp(attachment.StoreOp);

            // Depth Clear Value (Depth, Stencil)
            // Note: You might want to add a StencilClearValue to your struct later
            vkDepthAttachment.clearValue.depthStencil = vk::ClearDepthStencilValue(attachment.DepthClearValue, 0);
        }

        vk::RenderingInfo vkInfo;

        // Render Area
        vkInfo.renderArea = vk::Rect2D(
            { static_cast<int32_t>(info.RenderAreaX), static_cast<int32_t>(info.RenderAreaY) },
            { info.RenderAreaWidth, info.RenderAreaHeight }
        );

        vkInfo.layerCount = 1;

        // Link Color
        if (!vkColorAttachments.empty())
        {
            vkInfo.colorAttachmentCount = static_cast<uint32_t>(vkColorAttachments.size());
            vkInfo.pColorAttachments = vkColorAttachments.data();
        }

        // Link Depth
        if (hasDepth)
        {
            vkInfo.pDepthAttachment = &vkDepthAttachment;

            // If the format has stencil, we usually link stencil attachment to the same info
            // (Assuming Depth/Stencil are interleaved in the same image view)
            // vkInfo.pStencilAttachment = &vkDepthAttachment;
        }

        m_CommandBuffer.beginRendering(vkInfo);
        */
    }

    void CommandList::EndRendering()
    {
        m_CommandBuffer.endRendering();
    }

    void CommandList::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        // Vulkan Y-flip standard: negative height, y = y + height
        vk::Viewport viewport(x, y + height, width, -height, minDepth, maxDepth);
        m_CommandBuffer.setViewport(0, 1, &viewport);
    }

    void CommandList::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        vk::Rect2D scissor({x, y}, {width, height});
        m_CommandBuffer.setScissor(0, 1, &scissor);
    }

    void CommandList::BindPipeline(RHI::IPipeline* pipeline)
    {

    }

    void CommandList::BindVertexBuffer(RHI::IBuffer* buffer, uint32_t binding)
    {

    }

    void CommandList::BindIndexBuffer(RHI::IBuffer* buffer)
    {

    }

    void CommandList::PipelineBarrier(RHI::ITexture* texture, RHI::ResourceState oldState, RHI::ResourceState newState)
    {

    }

    void CommandList::PushConstants(RHI::IPipeline* pipeline, RHI::ShaderStage stage, const void* data, uint32_t size)
    {

    }

    void CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        m_CommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
        m_CommandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}
