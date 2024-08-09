#include "mxpch.hpp"
#include "VulkanRendererAPI.hpp"

#include "Platform/Vulkan/VulkanManager.hpp"

#include "Mixture/Core/Application.hpp"

namespace Mixture
{
    void VulkanRendererAPI::Init(const std::string& applicationName)
    {
        m_Context = &VulkanContext::Get();
        
        // TODO: custom allocator
        VulkanManager manager{};
        manager.Init();
        
        m_Context->Instance = CreateScope<VulkanInstance>(applicationName, manager);
        m_Context->Surface = CreateScope<VulkanSurface>(Application::Get().GetWindow());
        m_Context->DebugMessenger = CreateScope<VulkanDebugMessenger>();
        m_Context->PhysicalDevice = CreateScope<VulkanPhysicalDevice>();
        m_Context->Device = CreateScope<VulkanDevice>(manager);
        m_Context->SwapChain = CreateScope<VulkanSwapChain>();
        m_Context->GraphicsPipeline = CreateScope<VulkanGraphicsPipeline>();
    }

    VulkanRendererAPI::~VulkanRendererAPI()
    {
        m_Context->GraphicsPipeline = nullptr;
        m_Context->SwapChain = nullptr;
        m_Context->Device = nullptr;
        m_Context->PhysicalDevice = nullptr;
        m_Context->DebugMessenger = nullptr;
        m_Context->Surface = nullptr;
        m_Context->Instance = nullptr;
    }

    void VulkanRendererAPI::OnWindowResize(uint32_t width, uint32_t height)
    {
        // TODO: recreate swapchain
    }

    bool VulkanRendererAPI::BeginFrame()
    {
        return true;
    }

    bool VulkanRendererAPI::EndFrame()
    {
        return true;
    }
}
