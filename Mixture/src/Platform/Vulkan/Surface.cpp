#include "mxpch.hpp"
#include "Surface.hpp"

#include "Mixture/Core/Window.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    bool QueueFamilyIndices::IsComplete()
    {
        return Graphics.has_value() && Present.has_value();
    }

    Surface::Surface(const Window& window)
    {
        window.CreateSurface(Context::Get().Instance->GetHandle(), nullptr, &m_Surface);
    }

    Surface::~Surface()
    {
        if (m_Surface)
        {
            vkDestroySurfaceKHR(Context::Get().Instance->GetHandle(), m_Surface, nullptr);
            m_Surface = nullptr;
        }
    }

    QueueFamilyIndices Surface::FindQueueFamilyIndices(VkPhysicalDevice device) const
    {
        QueueFamilyIndices indices{};
        
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        
        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.Graphics = i;
            
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
            if (presentSupport) indices.Present = i;
            
            if (indices.IsComplete()) break;

            i++;
        }
        
        return indices;
    }

    SwapChainSupportDetails Surface::QuerySwapChainSupport(VkPhysicalDevice device) const
    {
        SwapChainSupportDetails details{};
        
        // Capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.Capabilities);
        
        // Formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount)
        {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.Formats.data());
        }
        
        // Present Modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount)
        {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.PresentModes.data());
        }
        
        return details;
    }
}
