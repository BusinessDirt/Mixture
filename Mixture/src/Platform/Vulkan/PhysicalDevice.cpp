#include "mxpch.hpp"
#include "PhysicalDevice.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Mixture/Util/VulkanUtil.hpp"

#include <set>

namespace Mixture::Vulkan
{
    namespace Util
    {
        static std::vector<VkExtensionProperties> GetAvailableExtensions(const VkPhysicalDevice physicalDevice)
        {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

            return availableExtensions;
        }
    }

    PhysicalDevice::PhysicalDevice(const std::vector<const char*>& requiredExtensions)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(Context::Get().GetInstance().GetHandle(), &deviceCount, nullptr);
        MX_CORE_ASSERT(deviceCount, "Failed to find GPU with Vulkan support!");
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(Context::Get().GetInstance().GetHandle(), &deviceCount, devices.data());
        
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto& device : devices)
        {
            int score = RateDeviceSuitability(device, requiredExtensions);
            candidates.insert(std::make_pair(score, device));
        }

        // Check if the best candidate is suitable at all
        if (candidates.rbegin()->first > 0)
        {
            m_PhysicalDevice = candidates.rbegin()->second;
            m_QueueFamilyIndices = FindQueueFamilyIndices(m_PhysicalDevice);

            Util::PrintDebugAvailability(Util::GetAvailableExtensions(m_PhysicalDevice), requiredExtensions, [](const VkExtensionProperties& extension) { return extension.extensionName; }, "Device Extensions");
        }
        
        MX_CORE_ASSERT(m_PhysicalDevice, "Failed to find a suitable GPU!");
        
        // Debug Information
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
        vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

        VULKAN_INFO_BEGIN("Selected GPU Information");
        VULKAN_INFO_LIST("Device Name: {}", 0, m_Properties.deviceName);
        //VULKAN_INFO_LIST("Device Type: {}", 0, vk::to_string(m_Properties.deviceType));
        //VULKAN_INFO_LIST("API Version: {}", 0, vk::to_string(m_Properties.apiVersion));
        //VULKAN_INFO_LIST("Driver Version: {}", 0, vk::to_string(m_Properties.driverVersion));
        VULKAN_INFO_LIST("Vendor ID: 0x{:X}", 0, m_Properties.vendorID);
        VULKAN_INFO_LIST("Device ID: 0x{:X}", 0, m_Properties.deviceID);
        VULKAN_INFO_LIST_HEADER("Limits:", 0);
        VULKAN_INFO_LIST("Max Image Dimension 2D: {}", 1, m_Properties.limits.maxImageDimension2D);
        VULKAN_INFO_LIST("Max Uniform Buffer Range: {}", 1, m_Properties.limits.maxUniformBufferRange);
        VULKAN_INFO_LIST("Max Viewports: {}", 1, m_Properties.limits.maxViewports);
        VULKAN_INFO_END();
    }

    int PhysicalDevice::RateDeviceSuitability(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions)
    {
        // Basic device properites like name, type, supported vulkan versions, etc
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        // Optional features like texture compression, multi viewport rendering, 64 it floats, etc
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // These features need to be supported for the application to work
        // ===============================================================

        // Graphics and Present Queue
        QueueFamilyIndices indices = FindQueueFamilyIndices(device);
        if (!indices.IsComplete()) score = 0;

        // Device Extensions need to be supported
        if (!CheckExtensionSupport(device, requiredExtensions)) score = 0;

        // Swapchain support (has to have atleast one format and present mode)
        SwapChainSupportDetails swapchainSupport = QuerySwapChainSupport(device);
        if (swapchainSupport.Formats.empty() || swapchainSupport.PresentModes.empty()) score = 0;

        return score;
    }

    bool PhysicalDevice::CheckExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> extensionSet(requiredExtensions.begin(), requiredExtensions.end());

        for (const auto& extension : availableExtensions)
        {
            extensionSet.erase(extension.extensionName);
        }

        return extensionSet.empty();
    }

    QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices(const VkPhysicalDevice device)
    {
        return Context::Get().GetSurface().FindQueueFamilyIndices(device);
    }

    SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(const VkPhysicalDevice device) const
    {
        return Context::Get().GetSurface().QuerySwapChainSupport(device ? device : m_PhysicalDevice);
    }
}
