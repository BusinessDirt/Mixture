#pragma once

/**
 * @file CommandList.hpp
 * @brief Vulkan implementation of the CommandList interface.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Mixture/Render/RHI/ICommandList.hpp"

namespace Mixture::Vulkan
{
    struct FrameCommandContext
    {
        vk::CommandBuffer graphicsCommandBuffer;
        vk::CommandBuffer transferCommandBuffer;
        vk::CommandBuffer computeCommandBuffer;
    };
    /**
     * @brief Vulkan implementation of the CommandList.
     *
     * Records commands into a Vulkan Command Buffer.
     */
    class CommandList : public RHI::ICommandList
    {
    public:
        CommandList(const FrameCommandContext& commandContext, vk::Image swapchainImage)
            : m_CommandContext(commandContext), m_SwapchainImage(swapchainImage) {}
        ~CommandList() = default;

        void Begin() override;
        void End() override;

        void BeginRendering(const RHI::RenderingInfo& info) override;
        void EndRendering() override;

        void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) override;
        void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) override;

        void BindPipeline(RHI::IPipeline* pipeline) override;
        void BindVertexBuffer(RHI::IBuffer* buffer, uint32_t binding = 0) override;
        void BindIndexBuffer(RHI::IBuffer* buffer) override;

        void PipelineBarrier(RHI::ITexture* texture, RHI::ResourceState oldState, RHI::ResourceState newState) override;
        void PushConstants(RHI::IPipeline* pipeline, RHI::ShaderStage stage, const void* data, uint32_t size) override;
        void SetUniformBuffer(uint32_t binding, RHI::IBuffer* buffer) override;
        void SetTexture(uint32_t binding, RHI::ITexture* texture) override;

        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;

    private:
        void FlushDescriptors(); // The magic function

    private:
        // Staging area for bindings
        struct BindingState
        {
            RHI::IBuffer* Buffer;
            RHI::ITexture* Texture;
            vk::DescriptorType Type;
        };

        FrameCommandContext m_CommandContext;
        vk::Image m_SwapchainImage;
        vk::PipelineLayout m_CurrentPipelineLayout;

        std::map<uint32_t, BindingState> m_Bindings;
        bool m_DescriptorsDirty = false;
    };
}
