#include "mxpch.hpp"
#include "Platform/Vulkan/Context.hpp"

#include <GLFW/glfw3.h>

namespace Mixture::Vulkan
{

    static Context* s_Instance = nullptr;

    Context& Context::Get()
    {
        return *s_Instance;
    }

    Context::Context(const ApplicationDescription& appDescription)
        : RHI::IGraphicsContext(appDescription)
    {
        s_Instance = this;

        m_Instance = CreateRef<Instance>(appDescription);
        m_PhysicalDevice = CreateRef<PhysicalDevice>(m_Instance->GetHandle());
        m_Device = CreateRef<Device>(m_Instance, m_PhysicalDevice);

        OPAL_INFO("Core/Vulkan", "Vulkan Initialized.")
    }

    Context::~Context()
    {
        m_Device.reset();
        m_PhysicalDevice.reset();
        m_Instance.reset();
    }
}
