#include "mxpch.hpp"
#include "Platform/Vulkan/VulkanContext.hpp"

#include <GLFW/glfw3.h>

namespace Mixture {

    static VulkanContext* s_Instance = nullptr;

    VulkanContext& VulkanContext::Get()
    {
        return *s_Instance;
    }

    VulkanContext::VulkanContext(const ApplicationDescription& appDescription)
        : RHI::IGraphicsContext(appDescription)
    {
        s_Instance = this;

        m_Instance = CreateRef<Instance>(appDescription);
        m_PhysicalDevice = CreateRef<PhysicalDevice>(m_Instance->GetHandle());
        m_Device = CreateRef<Device>(m_PhysicalDevice);
    }

    VulkanContext::~VulkanContext()
    {
        m_Device.reset();
        m_PhysicalDevice.reset();
        m_Instance.reset();
    }
}
