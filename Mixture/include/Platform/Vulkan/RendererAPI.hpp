#pragma once

#include "Mixture/Renderer/RendererAPI.hpp"
#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    class RendererAPI final : public Mixture::RendererAPI
    {
    public:
        void Initialize(const std::string& applicationName) override;
        void DestroyImGuiContext() override;
        void Shutdown() override;

        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void SetClearColor(const glm::vec4& color) override;

        void BeginFrame() override;
        void EndFrame() override;
		
        void BeginSceneRenderpass() override;
        void EndSceneRenderpass() override;
		
        void BeginImGuiImpl() override;
        void RenderImGui() override;

        void Draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
            const uint32_t firstInstance) override
        {
            vkCmdDraw(Context::CurrentCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
        }

        void DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex,
            const int32_t vertexOffset, const uint32_t firstInstance) override
        {
            vkCmdDrawIndexed(Context::CurrentCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

    private:
        static void RebuildSwapchain();
        static void CreateImGuiFrameBuffers();

        OPAL_NODISCARD VkCommandBuffer GetCurrentCommandBuffer() const
        {
            OPAL_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame is not in progress!")
            return Context::CommandBuffers->Get(Context::Swapchain->GetCurrentFrameIndex());
        }

    private:
        bool m_IsFrameStarted = false;
        VkClearColorValue m_ClearColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        
        Context m_Context;
    };
}