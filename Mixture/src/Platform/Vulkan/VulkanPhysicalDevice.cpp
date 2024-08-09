#include "mxpch.hpp"
#include "VulkanPhysicalDevice.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"

namespace Mixture
{
    VulkanPhysicalDevice::VulkanPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(VulkanContext::Get().Instance->GetHandle(), &deviceCount, nullptr);
        MX_CORE_ASSERT(deviceCount, "Failed to find GPU with Vulkan support!");
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(VulkanContext::Get().Instance->GetHandle(), &deviceCount, devices.data());
        
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
        return VulkanContext::Get().Surface->FindQueueFamilyIndices(m_PhysicalDevice);
    }

    VulkanSwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport() const
    {
        return VulkanContext::Get().Surface->QuerySwapChainSupport(m_PhysicalDevice);
    }

    bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
    {
        VulkanQueueFamilyIndices indices = VulkanContext::Get().Surface->FindQueueFamilyIndices(device);
        
        VulkanSwapChainSupportDetails swapChainSupport = VulkanContext::Get().Surface->QuerySwapChainSupport(device);
        bool swapChainAdequate = !(swapChainSupport.Formats.empty() && swapChainSupport.PresentModes.empty());
        
        return indices.IsComplete() && swapChainAdequate;
    }
}
