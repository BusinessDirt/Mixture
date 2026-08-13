#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file CommandList.hpp
 * @brief Metal implementation of the CommandList interface.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/ICommandList.hpp"

namespace Mixture::Metal
{
    class Device;

    /**
     * @brief Metal implementation of a command list.
     */
    class CommandList : public RHI::ICommandList
    {
    public:
        CommandList(MTL::CommandQueue* queue);
        ~CommandList();

        void Begin() override;
        void End() override;

        void BeginRendering(const RHI::RenderingInfo& info) override;
        void EndRendering() override;

        void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) override;
        void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) override;

        void BindPipeline(RHI::IPipeline* pipeline) override;
        void BindVertexBuffer(RHI::IBuffer* buffer, uint32_t binding = 0) override;
        void BindIndexBuffer(RHI::IBuffer* buffer) override;

        void PipelineBarrier(RHI::ITexture* texture, RHI::ResourceState oldState, RHI::ResourceState newState) override {}
        void PipelineBarrier(RHI::IBuffer* buffer, RHI::ResourceState oldState, RHI::ResourceState newState) override {}
        void PushConstants(RHI::IPipeline* pipeline, RHI::ShaderStage stage, const void* data, uint32_t size) override;
        void SetUniformBuffer(uint32_t binding, RHI::IBuffer* buffer, uint32_t set = 0) override;
        void SetTexture(uint32_t binding, RHI::ITexture* texture, uint32_t set = 0) override;

        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;

        MTL::CommandBuffer* GetCommandBuffer() const { return m_CommandBuffer; }

    private:
        MTL::CommandQueue* m_Queue = nullptr;
        MTL::CommandBuffer* m_CommandBuffer = nullptr;
        MTL::RenderCommandEncoder* m_Encoder = nullptr;
        MTL::Buffer* m_IndexBuffer = nullptr;
    };
}

#endif
