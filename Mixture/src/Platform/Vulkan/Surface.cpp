#include "mxpch.hpp"
#include "Surface.hpp"

#include "Mixture/Core/Window.hpp"
#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    bool QueueFamilyIndices::IsComplete()
    {
        return Graphics.has_value() && Present.has_value();
    }

    Surface::Surface()
    {
        Application::Get().GetWindow().CreateSurface(Context::Get().GetInstance().GetHandle(), nullptr, &m_Surface);
    }

    Surface::~Surface()
    {
        if (m_Surface)
        {
            vkDestroySurfaceKHR(Context::Get().GetInstance().GetHandle(), m_Surface, nullptr);
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
            // Graphics Queue
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.Graphics = i;
            
            // Present Queue
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
            if (presentSupport) indices.Present = i;
            
            // Break early if all indices have been found
            if (indices.IsComplete()) break;

            i++;
        }
        
        return indices;
    }

    SwapChainSupportDetails Surface::QuerySwapChainSupport(VkPhysicalDevice device) const
    {
        SwapChainSupportDetails details{};
        
        // Basic surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.Capabilities);
        
        // Supported surface formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount)
        {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.Formats.data());
        }
        
        // Supported presentation modes
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
