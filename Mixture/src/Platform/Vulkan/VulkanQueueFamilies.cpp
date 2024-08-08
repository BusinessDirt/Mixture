#include "mxpch.hpp"
#include "VulkanQueueFamilies.hpp"

namespace Mixture
{
    bool QueueFamilyIndices::IsComplete()
    {
        return Graphics.has_value();
    }

    QueueFamilyIndices VulkanQueueFamilies::Find(VkPhysicalDevice device)
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
            
            if (indices.IsComplete()) break;

            i++;
        }
        
        return indices;
    }
}
