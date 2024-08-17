#include "mxpch.hpp"
#include "PhysicalDevice.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    PhysicalDevice::PhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(Context::Get().Instance->GetHandle(), &deviceCount, nullptr);
        MX_CORE_ASSERT(deviceCount, "Failed to find GPU with Vulkan support!");
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(Context::Get().Instance->GetHandle(), &deviceCount, devices.data());
        
        for (const auto& device : devices)
        {
            if (PhysicalDevice::IsDeviceSuitable(device))
            {
                m_PhysicalDevice = device;
                break;
            }
        }
        
        MX_CORE_ASSERT(m_PhysicalDevice, "Failed to find a suitable GPU!");
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
    }

    QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices() const
    {
        return Context::Get().Surface->FindQueueFamilyIndices(m_PhysicalDevice);
    }

    SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport() const
    {
        return Context::Get().Surface->QuerySwapChainSupport(m_PhysicalDevice);
    }

    bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = Context::Get().Surface->FindQueueFamilyIndices(device);
        
        SwapChainSupportDetails swapChainSupport = Context::Get().Surface->QuerySwapChainSupport(device);
        bool swapChainAdequate = !(swapChainSupport.Formats.empty() && swapChainSupport.PresentModes.empty());
        
        // TODO: instead of forcing this disable anisotropic filtering
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
        
        return indices.IsComplete() && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }
}
