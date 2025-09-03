#include "mxpch.hpp"
#include "Platform/Vulkan/WindowSurface.hpp"

#include "Platform/Vulkan/Context.hpp"

#include "Mixture/Core/Application.hpp"

namespace Mixture::Vulkan
{
    WindowSurface::WindowSurface()
    {
        const Window& window = Application::Get().GetWindow();
        window.CreateVulkanSurface(Context::Instance->GetHandle(), nullptr, &m_Surface);
    }

    WindowSurface::~WindowSurface()
    {
        if (m_Surface)
        {
            vkDestroySurfaceKHR(Context::Instance->GetHandle(), m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }
    }
}
