#include "mxpch.hpp"
#include "RendererAPI.hpp"

#include "Platform/Vulkan/Manager.hpp"

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
        m_Context->SwapChain = CreateScope<SwapChain>();
        m_Context->RenderPass = CreateScope<RenderPass>();
        m_Context->GraphicsPipeline = CreateScope<GraphicsPipeline>();
    }

    RendererAPI::~RendererAPI()
    {
        m_Context->GraphicsPipeline = nullptr;
        m_Context->RenderPass = nullptr;
        m_Context->SwapChain = nullptr;
        m_Context->Device = nullptr;
        m_Context->PhysicalDevice = nullptr;
        m_Context->DebugMessenger = nullptr;
        m_Context->Surface = nullptr;
        m_Context->Instance = nullptr;
    }

    void RendererAPI::OnWindowResize(uint32_t width, uint32_t height)
    {
        // TODO: recreate swapchain
    }

    bool RendererAPI::BeginFrame()
    {
        return true;
    }

    bool RendererAPI::EndFrame()
    {
        return true;
    }
}
