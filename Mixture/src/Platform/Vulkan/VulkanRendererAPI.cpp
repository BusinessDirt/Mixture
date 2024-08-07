#include "mxpch.hpp"
#include "VulkanRendererAPI.hpp"

namespace Mixture
{
    void VulkanRendererAPI::Init(const std::string& applicationName)
    {
        // TODO: custom allocator
        m_Instance = CreateScope<VulkanInstance>(applicationName);
        m_DebugMessenger = CreateScope<VulkanDebugMessenger>(*m_Instance);
    }

    VulkanRendererAPI::~VulkanRendererAPI()
    {
        m_DebugMessenger = nullptr;
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
