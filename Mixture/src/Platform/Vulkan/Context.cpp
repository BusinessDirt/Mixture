#include "mxpch.hpp"
#include "Platform/Vulkan/Context.hpp"

#include "Mixture/Core/Application.hpp"

#include <GLFW/glfw3.h>

namespace Mixture::Vulkan
{

    static Context* s_Instance = nullptr;

    Context& Context::Get()
    {
        return *s_Instance;
    }

    Context::Context(const ApplicationDescription& appDescription, void* windowHandle)
        : RHI::IGraphicsContext(appDescription, windowHandle)
    {
        s_Instance = this;

        m_Instance = CreateRef<Instance>(appDescription);
        m_Surface = CreateRef<Surface>(m_Instance, windowHandle);
        m_PhysicalDevice = CreateRef<PhysicalDevice>(m_Instance->GetHandle());
        m_Device = CreateRef<Device>(m_Instance, m_PhysicalDevice);
        m_Swapchain = CreateRef<Swapchain>(m_PhysicalDevice, m_Device, m_Surface, appDescription.Width, appDescription.Height);

        OPAL_INFO("Core/Vulkan", "Vulkan Initialized.");
    }

    Context::~Context()
    {
        m_Device->WaitForIdle();

        m_Swapchain.reset();
        m_Device.reset();
        m_PhysicalDevice.reset();
        m_Surface.reset();
        m_Instance.reset();
    }
}
