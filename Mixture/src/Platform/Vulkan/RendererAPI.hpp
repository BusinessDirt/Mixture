#pragma once

#include "Mixture/Renderer/RendererAPI.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Command/CommandBuffers.hpp"

namespace Mixture::Vulkan
{

    class RendererAPI : public ::Mixture::RendererAPI
    {
    public:
        ~RendererAPI() override;

        void Init(const std::string& applicationName) override;

        void OnWindowResize(uint32_t width, uint32_t height) override;

        CommandBuffer BeginFrame() override;
        void BeginRenderPass(CommandBuffer commandBuffer) override;
        void EndRenderPass(CommandBuffer commandBuffer) override;
        void EndFrame(CommandBuffer commandBuffer) override;
        void SubmitFrame(const std::vector<CommandBuffer>& commandBuffers) override;
        void WaitForDevice() override;
        
    private:
        CommandBuffer GetCurrentCommandBuffer() const
        {
            MX_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");
            return m_CommandBuffers->Get(m_Context->SwapChain->GetCurrentFrameIndex());
        }
        
        void RebuildSwapChain();
        
    private:
        Context* m_Context;
        Scope<CommandBuffers> m_CommandBuffers;
        
        bool m_IsFrameStarted = false;
    };
}
