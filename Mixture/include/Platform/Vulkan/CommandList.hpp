#pragma once

/**
 * @file CommandList.hpp
 * @brief Vulkan implementation of the CommandList interface.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/ICommandList.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Vulkan implementation of the CommandList.
     * 
     * Records commands into a Vulkan Command Buffer.
     */
    class CommandList : public RHI::ICommandList
    {
    public:
        CommandList(vk::CommandBuffer commandBuffer, vk::Image swapchainImage)
            : m_CommandBuffer(commandBuffer), m_SwapchainImage(swapchainImage) {}
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

        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;

        vk::CommandBuffer GetHandle() const { return m_CommandBuffer; }

    private:
        vk::CommandBuffer m_CommandBuffer;
        vk::Image m_SwapchainImage;
    };
}
