#include "mxpch.hpp"
#include "RendererAPI.hpp"

#include "Platform/Vulkan/Manager.hpp"
#include "Platform/Vulkan/Buffer/FrameBuffer.hpp"

#include "Mixture/Core/Application.hpp"

namespace Mixture::Vulkan
{
    void RendererAPI::Init(const std::string& applicationName)
    {
        m_Context = &Context::Get();
        
        // TODO: custom allocator
        Manager manager{};
        manager.Init();
        
        m_Context->Instance = CreateScope<Instance>(applicationName, manager);
        m_Context->Surface = CreateScope<Surface>(Application::Get().GetWindow());
        m_Context->DebugMessenger = CreateScope<DebugMessenger>();
        m_Context->PhysicalDevice = CreateScope<PhysicalDevice>();
        m_Context->Device = CreateScope<Device>(manager);
        m_Context->CommandPool = CreateScope<CommandPool>();
        m_Context->SwapChain = CreateScope<SwapChain>();
        
        

        m_CommandBuffers = CreateScope<CommandBuffers>(SwapChain::MAX_FRAMES_IN_FLIGHT);
    }

    RendererAPI::~RendererAPI()
    {
        if (m_Context->ImGuiViewport) m_Context->ImGuiViewport = nullptr;
        m_CommandBuffers = nullptr;
        m_Context->DescriptorSetManager = nullptr;
        m_Context->CommandPool = nullptr;
        m_Context->SwapChain = nullptr;
        m_Context->Device = nullptr;
        m_Context->PhysicalDevice = nullptr;
        m_Context->DebugMessenger = nullptr;
        m_Context->Surface = nullptr;
        m_Context->Instance = nullptr;
    }

    void RendererAPI::RebuildSwapChain()
    {
        vkDeviceWaitIdle(Context::Get().Device->GetHandle());

        Ref<SwapChain> oldSwapChain = std::move(Context::Get().SwapChain);
        Context::Get().SwapChain = CreateScope<SwapChain>(oldSwapChain);

        MX_CORE_ASSERT(oldSwapChain->CompareSwapFormats(*Context::Get().SwapChain.get()),
                       "Swap chain image(or depth) format has changed!");
    }

    void RendererAPI::OnWindowResize(uint32_t width, uint32_t height)
    {
        RebuildSwapChain();
    }

    void RendererAPI::WaitForDevice()
    {
        vkDeviceWaitIdle(Context::Get().Device->GetHandle());
    }

    CommandBuffer RendererAPI::BeginFrame()
    {
        MX_CORE_ASSERT(!m_IsFrameStarted, "Can't call BeginFrame() while already in progress");
        
        VkResult result = Context::Get().SwapChain->AcquireNextImage();
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RebuildSwapChain();
            return VK_NULL_HANDLE;
        }

        MX_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image");
        
        m_IsFrameStarted = true;
        
        CommandBuffer commandBuffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        MX_VK_ASSERT(vkBeginCommandBuffer(commandBuffer.GetAsVulkanHandle(), &beginInfo),
            "Failed to begin recording command buffer");
        
        return commandBuffer;
    }

    void RendererAPI::EndFrame(CommandBuffer commandBuffer)
    {
        MX_CORE_ASSERT(m_IsFrameStarted, "Can't call EndFrame() while frame is not in progress");
        MX_VK_ASSERT(vkEndCommandBuffer(commandBuffer.GetAsVulkanHandle()), "Failed to record command buffer");
    }

    void RendererAPI::SubmitFrame(const std::vector<CommandBuffer>& commandBuffers)
    {
        VkResult result = Context::Get().SwapChain->SubmitCommandBuffers(commandBuffers);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RebuildSwapChain();
        }
        else
        {
            MX_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swap chain image");
        }

        m_IsFrameStarted = false;
    }

    void RendererAPI::BeginRenderPass(CommandBuffer commandBuffer)
    {
        MX_CORE_ASSERT(m_IsFrameStarted, "Can't call BeginRenderPass() if frame is not in progress");
        
        const SwapChain& swapChain = *m_Context->SwapChain;
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_Context->SwapChain->GetRenderPass().GetHandle();
        renderPassInfo.framebuffer = swapChain.GetFrameBuffer(m_Context->CurrentImageIndex).GetHandle();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        
        VkExtent2D swapChainExtent = swapChain.GetExtent();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer.GetAsVulkanHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{ {0, 0}, swapChainExtent };

        vkCmdSetViewport(commandBuffer.GetAsVulkanHandle(), 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer.GetAsVulkanHandle(), 0, 1, &scissor);
    }

    void RendererAPI::EndRenderPass(CommandBuffer commandBuffer)
    {
        MX_CORE_ASSERT(m_IsFrameStarted, "Can't call EndRenderPass() if frame is not in progress");
        MX_CORE_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer.GetAsVulkanHandle());
    }
}
