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
        vkDestroySurfaceKHR(Context::Instance->GetHandle(), m_Surface, nullptr);
    }
}
