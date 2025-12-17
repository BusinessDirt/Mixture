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

        m_Instance = CreateRef<Instance>();
        m_PhysicalDevice = CreateRef<PhysicalDevice>(m_Instance->GetHandle());
    }

    VulkanContext::~VulkanContext()
    {

    }
}
