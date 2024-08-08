#include "mxpch.hpp"
#include "VulkanPhysicalDevice.hpp"

#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"

namespace Mixture
{
    VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface)
        : m_Instance(instance), m_Surface(surface)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance.GetHandle(), &deviceCount, nullptr);
        MX_CORE_ASSERT(deviceCount, "Failed to find GPU with Vulkan support!");
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance.GetHandle(), &deviceCount, devices.data());
        
        for (const auto& device : devices)
        {
            if (VulkanPhysicalDevice::IsDeviceSuitable(device))
            {
                m_PhysicalDevice = device;
                break;
            }
        }
        
        MX_CORE_ASSERT(m_PhysicalDevice, "Failed to find a suitable GPU!");
    }

    VulkanQueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilyIndices() const
    {
        return m_Surface.FindQueueFamilyIndices(m_PhysicalDevice);
    }

    VulkanSwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport() const
    {
        return m_Surface.QuerySwapChainSupport(m_PhysicalDevice);
    }

    bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
    {
        VulkanQueueFamilyIndices indices = m_Surface.FindQueueFamilyIndices(device);
        
        VulkanSwapChainSupportDetails swapChainSupport = m_Surface.QuerySwapChainSupport(device);
        bool swapChainAdequate = !(swapChainSupport.Formats.empty() && swapChainSupport.PresentModes.empty());
        
        return indices.IsComplete() && swapChainAdequate;
    }
}
