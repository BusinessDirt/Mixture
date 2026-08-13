#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Context.hpp"
#include "Mixture/Core/Application.hpp"

namespace Mixture::Metal
{
    static Context* s_Instance = nullptr;

    Context& Context::Get()
    {
        if (!s_Instance) throw std::logic_error("No Metal context exists");
        return *s_Instance;
    }

    Context::Context(const ApplicationDescription& appDescription, void* windowHandle)
    {
        s_Instance = this;
        m_Device = CreateRef<Device>();
        m_Swapchain = CreateScope<Swapchain>(*m_Device, windowHandle, appDescription.Width, appDescription.Height);
    }

    Context::~Context()
    {
        m_Swapchain.reset();
        m_Device.reset();
        s_Instance = nullptr;
    }

    RHI::IGraphicsDevice& Context::GetDevice() const
    {
        return *m_Device;
    }

    void Context::OnResize(uint32_t width, uint32_t height)
    {
        if (m_Swapchain)
        {
            m_Swapchain->Resize(width, height);
        }
    }

    RHI::ITexture* Context::BeginFrame()
    {
        if (m_Swapchain)
        {
            return m_Swapchain->AcquireNextTexture();
        }
        return nullptr;
    }

    void Context::EndFrame()
    {
        if (m_Swapchain)
        {
            m_Swapchain->Present();
        }
        m_CurrentFrame = (m_CurrentFrame + 1) % 2;
    }

    Scope<RHI::ICommandList> Context::GetCommandBuffer()
    {
        if (m_Device && m_Device->GetCommandQueue())
        {
            return CreateScope<CommandList>(m_Device->GetCommandQueue());
        }
        return nullptr;
    }

    uint32_t Context::GetSwapchainWidth() const
    {
        return m_Swapchain ? m_Swapchain->GetWidth() : 0;
    }

    uint32_t Context::GetSwapchainHeight() const
    {
        return m_Swapchain ? m_Swapchain->GetHeight() : 0;
    }
}

#endif
