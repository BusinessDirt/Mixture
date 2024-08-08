#include "mxpch.hpp"
#include "VulkanRendererAPI.hpp"

#include "Platform/Vulkan/VulkanManager.hpp"

#include "Mixture/Core/Application.hpp"

namespace Mixture
{
    void VulkanRendererAPI::Init(const std::string& applicationName)
    {
        // TODO: custom allocator
        VulkanManager manager{};
        manager.Init();
        
        m_Instance = CreateScope<VulkanInstance>(applicationName, manager);
        m_Surface = CreateScope<VulkanSurface>(Application::Get().GetWindow(), *m_Instance);
        m_DebugMessenger = CreateScope<VulkanDebugMessenger>(*m_Instance);
        m_PhysicalDevice = CreateScope<VulkanPhysicalDevice>(*m_Instance, *m_Surface);
        m_Device = CreateScope<VulkanDevice>(*m_PhysicalDevice, manager);
    }

    VulkanRendererAPI::~VulkanRendererAPI()
    {
        m_Device = nullptr;
        m_PhysicalDevice = nullptr;
        m_DebugMessenger = nullptr;
        m_Surface = nullptr;
        m_Instance = nullptr;
    }

    void VulkanRendererAPI::OnWindowResize(uint32_t width, uint32_t height)
    {
        
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
