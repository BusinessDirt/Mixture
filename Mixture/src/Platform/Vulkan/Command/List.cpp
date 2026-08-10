#include "mxpch.hpp"
#include "Platform/Vulkan/Command/List.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Resources/Texture.hpp"
#include "Platform/Vulkan/Resources/Buffer.hpp"
#include "Platform/Vulkan/Descriptors/Builder.hpp"

#include <set>
#include "Platform/Vulkan/Pipeline/Pipeline.hpp"
#include "Platform/Vulkan/ResourcePolicy.hpp"

namespace Mixture::Vulkan
{
    namespace
    {
        namespace Utils
        {
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
                    0, nullptr, 0, nullptr,
                    1, &barrier
                );
            }
        }
    }

    void CommandList::Begin()
    {
        m_IsPipelineBound = false;
        if (m_CommandContext.Activity) m_CommandContext.Activity->Graphics = true;

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit; // Reset every frame
        m_CommandContext.transferCommandBuffer.begin(beginInfo);
        m_CommandContext.computeCommandBuffer.begin(beginInfo);
        m_CommandContext.graphicsCommandBuffer.begin(beginInfo);

        Utils::InsertImageBarrier(
            m_CommandContext.graphicsCommandBuffer, m_SwapchainImage,
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
            m_CommandContext.graphicsCommandBuffer, m_SwapchainImage,
            vk::ImageLayout::eColorAttachmentOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::AccessFlagBits::eColorAttachmentWrite,
            vk::AccessFlags() // Presentation reads implicitly
        );

        m_CommandContext.transferCommandBuffer.end();
        m_CommandContext.computeCommandBuffer.end();
        m_CommandContext.graphicsCommandBuffer.end();
    }

    void CommandList::BeginRendering(const RHI::RenderingInfo& info)
    {
        if (info.ColorAttachments.empty())
        {
            OPAL_WARN("Core/Vulkan", "BeginRendering called with NO Color Attachments!");
        }

        Vector<vk::RenderingAttachmentInfo> vkColorAttachments;
        vkColorAttachments.reserve(info.ColorAttachments.size());

        for (const auto& attachment : info.ColorAttachments)
        {
            if (!attachment.Image) continue;

            // Cast abstract ITexture to concrete VulkanTexture to get the view
            // NOTE: Ensure your Vulkan::Texture class has GetImageView()
            auto* vulkanTexture = static_cast<Texture*>(attachment.Image);

            vk::RenderingAttachmentInfo vkInfo;
            vkInfo.imageView = vulkanTexture->GetImageView();
            vkInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal; // Or infer from texture usage

            // Operations
            vkInfo.loadOp = EnumMapper::MapLoadOp(attachment.LoadOp);
            vkInfo.storeOp = EnumMapper::MapStoreOp(attachment.StoreOp);

            // Clear Color (R, G, B, A)
            vkInfo.clearValue.color = std::array<float, 4>
            {
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
            auto* vulkanTexture = static_cast<Texture*>(attachment.Image);

            vkDepthAttachment.imageView = vulkanTexture->GetImageView();
            vkDepthAttachment.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vkDepthAttachment.loadOp = EnumMapper::MapLoadOp(attachment.LoadOp);
            vkDepthAttachment.storeOp = EnumMapper::MapStoreOp(attachment.StoreOp);

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

        m_CommandContext.graphicsCommandBuffer.beginRendering(vkInfo);
    }

    void CommandList::EndRendering()
    {
        m_CommandContext.graphicsCommandBuffer.endRendering();
    }

    void CommandList::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        // Vulkan Y-flip standard: negative height, y = y + height
        vk::Viewport viewport(x, y + height, width, -height, minDepth, maxDepth);
        m_CommandContext.graphicsCommandBuffer.setViewport(0, 1, &viewport);
    }

    void CommandList::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        vk::Rect2D scissor({x, y}, {width, height});
        m_CommandContext.graphicsCommandBuffer.setScissor(0, 1, &scissor);
    }

    void CommandList::BindPipeline(RHI::IPipeline* pipeline)
    {
        if (!pipeline)
        {
            m_IsPipelineBound = false;
            m_CurrentPipeline = nullptr;
            return;
        }

        m_IsPipelineBound = true;
        auto* vkPipeline = static_cast<Pipeline*>(pipeline);
        m_CurrentPipeline = vkPipeline;
        m_CurrentPipelineLayout = vkPipeline->GetLayout();
        m_CommandContext.graphicsCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline->GetHandle());
    }

    void CommandList::BindVertexBuffer(RHI::IBuffer* buffer, uint32_t binding)
    {
        // Safety check (or assert) that usage is correct
        if (buffer->GetUsage() != RHI::BufferUsage::Vertex)
        {
            OPAL_ERROR("Core/Vulkan", "Trying to bind non-vertex buffer as vertex buffer!");
            return;
        }

        auto vkBuffer = static_cast<Buffer*>(buffer);

        vk::Buffer buffers[] = { vkBuffer->GetHandle() };
        vk::DeviceSize offsets[] = { 0 };

        m_CommandContext.graphicsCommandBuffer.bindVertexBuffers(binding, 1, buffers, offsets);
    }

    void CommandList::BindIndexBuffer(RHI::IBuffer* buffer)
    {
        if (buffer->GetUsage() != RHI::BufferUsage::Index)
        {
            OPAL_ERROR("Core/Vulkan", "Trying to bind non-index buffer as index buffer!");
            return;
        }

        auto vkBuffer = static_cast<Buffer*>(buffer);

        // Default to 32-bit indices for simplicity
        m_CommandContext.graphicsCommandBuffer.bindIndexBuffer(vkBuffer->GetHandle(), 0, vk::IndexType::eUint32);
    }

    void CommandList::PipelineBarrier(RHI::ITexture* texture, RHI::ResourceState oldState, RHI::ResourceState newState)
    {
        if (!texture || oldState == newState) return;
        const auto before = MapResourceState(oldState);
        const auto after = MapResourceState(newState);
        auto* vulkanTexture = static_cast<Texture*>(texture);

        vk::ImageMemoryBarrier barrier;
        barrier.srcAccessMask = before.Access;
        barrier.dstAccessMask = after.Access;
        barrier.oldLayout = before.Layout;
        barrier.newLayout = after.Layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = vulkanTexture->GetImage();
        barrier.subresourceRange = vk::ImageSubresourceRange(
            GetImageAspect(texture->GetFormat()), 0, 1, 0, 1);

        m_CommandContext.graphicsCommandBuffer.pipelineBarrier(
            before.Stages, after.Stages, {}, {}, {}, barrier);
    }

    void CommandList::PipelineBarrier(RHI::IBuffer* buffer, RHI::ResourceState oldState, RHI::ResourceState newState)
    {
        if (!buffer || oldState == newState) return;
        const auto before = MapResourceState(oldState);
        const auto after = MapResourceState(newState);
        auto* vulkanBuffer = static_cast<Buffer*>(buffer);

        vk::BufferMemoryBarrier barrier;
        barrier.srcAccessMask = before.Access;
        barrier.dstAccessMask = after.Access;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = vulkanBuffer->GetHandle();
        barrier.offset = 0;
        barrier.size = VK_WHOLE_SIZE;
        m_CommandContext.graphicsCommandBuffer.pipelineBarrier(
            before.Stages, after.Stages, {}, {}, barrier, {});
    }

    void CommandList::PushConstants(RHI::IPipeline* pipeline, RHI::ShaderStage stage, const void* data, uint32_t size)
    {
        if (!pipeline || !data || size == 0) return;
        auto* vulkanPipeline = static_cast<Pipeline*>(pipeline);
        const vk::ShaderStageFlags stages = EnumMapper::MapShaderStage(stage);
        const auto* range = vulkanPipeline->FindPushConstantRange(stages, size);
        if (!range)
        {
            OPAL_ERROR("Core/Vulkan", "Push constant write is not covered by the pipeline layout");
            return;
        }
        m_CommandContext.graphicsCommandBuffer.pushConstants(
            vulkanPipeline->GetLayout(), stages, range->offset, size, data);
    }

    void CommandList::SetUniformBuffer(uint32_t binding, RHI::IBuffer* buffer, uint32_t set)
    {
        m_Bindings[{ set, binding }].Buffer = buffer;
        m_Bindings[{ set, binding }].Texture = nullptr;
        m_Bindings[{ set, binding }].Type = vk::DescriptorType::eUniformBuffer;

        m_DescriptorsDirty = true;
    }

    void CommandList::SetTexture(uint32_t binding, RHI::ITexture* texture, uint32_t set)
    {
        m_Bindings[{ set, binding }].Buffer = nullptr;
        m_Bindings[{ set, binding }].Texture = texture;
        m_Bindings[{ set, binding }].Type = vk::DescriptorType::eCombinedImageSampler;

        m_DescriptorsDirty = true;
    }

    void CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        if (!m_IsPipelineBound) return;

        FlushDescriptors();
        m_CommandContext.graphicsCommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
        if (!m_IsPipelineBound) return;

        FlushDescriptors();
        m_CommandContext.graphicsCommandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandList::FlushDescriptors()
    {
        if (!m_DescriptorsDirty || m_Bindings.empty()) return;

        auto& context = Context::Get();

        // Get the Allocator for the CURRENT frame
        // (Assuming you added a getter to Context for the current frame's allocator)
        auto* allocator = context.GetCurrentDescriptorAllocator();
        auto* cache = context.GetDescriptorLayoutCache();

        std::set<uint32_t> usedSets;
        for (const auto& [location, state] : m_Bindings)
        {
            (void)state;
            usedSets.insert(location.first);
        }

        for (const uint32_t set : usedSets)
        {
            auto builder = DescriptorBuilder::Begin(allocator, cache);
            for (auto& [location, state] : m_Bindings)
            {
                if (location.first != set) continue;
                const uint32_t binding = location.second;
                if (state.Buffer)
                {
                    auto* vkBuf = static_cast<Buffer*>(state.Buffer);
                    vk::DescriptorBufferInfo info(vkBuf->GetHandle(), 0, vkBuf->GetSize());
                    builder.BindBuffer(binding, info, state.Type,
                        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
                }
                else if (state.Texture)
                {
                    auto* vkTex = static_cast<Texture*>(state.Texture);
                    builder.BindImage(binding, vkTex->GetDescriptorInfo(), state.Type, vk::ShaderStageFlagBits::eFragment);
                }
            }

            vk::DescriptorSet newSet;
            const auto setLayout = m_CurrentPipeline ? m_CurrentPipeline->GetDescriptorSetLayout(set) : vk::DescriptorSetLayout{};
            if (setLayout && builder.BuildWithLayout(newSet, setLayout))
            {
                m_CommandContext.graphicsCommandBuffer.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics, m_CurrentPipelineLayout,
                    set, 1, &newSet, 0, nullptr);
            }
        }

        // Reset dirty flag so we don't rebuild if nothing changed next draw
        m_DescriptorsDirty = false;
    }
}
