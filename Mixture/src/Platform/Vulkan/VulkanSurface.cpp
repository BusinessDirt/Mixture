#include "mxpch.hpp"
#include "VulkanSurface.hpp"

#include "Mixture/Core/Window.hpp"

#include "Platform/Vulkan/VulkanInstance.hpp"

namespace Mixture
{
    bool VulkanQueueFamilyIndices::IsComplete()
    {
        return Graphics.has_value() && Present.has_value();
    }

    VulkanSurface::VulkanSurface(const Window& window, const VulkanInstance& instance)
        : m_Instance(instance)
    {
        window.CreateSurface(instance.GetHandle(), nullptr, &m_Surface);
    }

    VulkanSurface::~VulkanSurface()
    {
        if (m_Surface)
        {
            vkDestroySurfaceKHR(m_Instance.GetHandle(), m_Surface, nullptr);
            m_Surface = nullptr;
        }
    }

    VulkanQueueFamilyIndices VulkanSurface::FindQueueFamilyIndices(VkPhysicalDevice device) const
    {
        VulkanQueueFamilyIndices indices{};
        
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
}
