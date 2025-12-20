#include "mxpch.hpp"
#include "Platform/Vulkan/Surface.hpp"

#include <glfw/glfw3.h>

namespace Mixture::Vulkan
{
    Surface::Surface(Ref<Instance> instance, void* windowHandle)
        : m_Instance(instance)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(windowHandle);

        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(m_Instance->GetHandle(), window, nullptr, &rawSurface) != VK_SUCCESS)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to create Window Surface!");
            exit(-1);
        }

        m_Handle = rawSurface;
    }

    Surface::~Surface()
    {
        if (m_Handle) m_Instance->GetHandle().destroySurfaceKHR(m_Handle);
    }
}
