#pragma once

#include "Mixture/Renderer/RendererAPI.hpp"

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

        bool BeginFrame() override;
        bool EndFrame() override;
        void WaitForDevice() override;
        
        
    private:
        VkCommandBuffer Begin();
        void End();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
        
        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            MX_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");
            return m_CommandBuffers->operator[](m_CurrentFrameIndex);
        }
        
        void RebuildSwapChain();
        
    private:
        Context* m_Context;
        Scope<CommandBuffers> m_CommandBuffers;
        
        uint32_t m_CurrentImageIndex = 0;
        int m_CurrentFrameIndex = 0;
        bool m_IsFrameStarted = false;
        bool m_RebuildSwapChain = false;
    };
}
