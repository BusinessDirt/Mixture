#include "mxpch.hpp"
#include "VulkanPhysicalDevice.hpp"

#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanQueueFamilies.hpp"

namespace Mixture
{
    VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstance& instance)
        : m_Instance(instance)
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

    bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = VulkanQueueFamilies::Find(device);
        
        return indices.IsComplete();
    }
}
