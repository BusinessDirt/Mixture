#include "mxpch.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include "Platform/Vulkan/Context.hpp"

#include <Opal/Base.hpp>

#include <set>

namespace Mixture::Vulkan
{
    namespace Util
    {
        namespace
        {
            std::vector<VkExtensionProperties> GetAvailableExtensions(const VkPhysicalDevice physicalDevice)
            {
                uint32_t extensionCount;
                vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

                std::vector<VkExtensionProperties> availableExtensions(extensionCount);
                vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
            
                return availableExtensions;
            } 
        }
    }

    PhysicalDevice::PhysicalDevice(const std::vector<const char*>& requiredExtensions)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(Context::Instance->GetHandle(), &deviceCount, nullptr);
        
        OPAL_ASSERT("Core", deviceCount > 0, "Mixture::Vulkan::PhysicalDevice::PhysicalDevice() - Failed to find GPU with Vulkan support!")
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(Context::Instance->GetHandle(), &deviceCount, devices.data());
        
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
        
        OPAL_ASSERT("Core", m_PhysicalDevice != VK_NULL_HANDLE,
                         "Mixture::Vulkan::PhysicalDevice::PhysicalDevice() - Failed to find suitable GPU!")
        
        // Debug Information
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
        vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

        VULKAN_INFO_BEGIN("Selected GPU Information");
        VULKAN_INFO_LIST("Device Name: {}", 0, m_Properties.deviceName);
        VULKAN_INFO_LIST("Device Type: {}", 0, ToString::PhysicalDeviceType(m_Properties.deviceType));
        VULKAN_INFO_LIST("API Version: {}", 0, ToString::Version(m_Properties.apiVersion));
        VULKAN_INFO_LIST("Driver Version: {}", 0, ToString::Version(m_Properties.driverVersion));
        VULKAN_INFO_LIST("Vendor ID: 0x{:X}", 0, m_Properties.vendorID);
        VULKAN_INFO_LIST("Device ID: 0x{:X}", 0, m_Properties.deviceID);
        VULKAN_INFO_LIST_HEADER("Limits:", 0);
        VULKAN_INFO_LIST("Max Image Dimension 2D: {}", 1, m_Properties.limits.maxImageDimension2D);
        VULKAN_INFO_LIST("Max Uniform Buffer Range: {}", 1, m_Properties.limits.maxUniformBufferRange);
        VULKAN_INFO_LIST("Max Viewports: {}", 1, m_Properties.limits.maxViewports);
        VULKAN_INFO_END();

    }

    int PhysicalDevice::RateDeviceSuitability(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions) const
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
        score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);
        
        // These features need to be supported for the application to work
        // ===============================================================
        
        // Graphics and Present Queue
        if (QueueFamilyIndices indices = FindQueueFamilyIndices(device); !indices.IsComplete()) score = 0;
        
        // Device Extensions need to be supported
        if (!CheckExtensionSupport(device, requiredExtensions))
            score = 0;
        
        // Swapchain support (has to have atleast one format and present mode)
        if (SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device);
            swapchainSupport.Formats.empty() || swapchainSupport.PresentModes.empty())
            score = 0;

        return score;
    }

    bool PhysicalDevice::CheckExtensionSupport(const VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> extensionSet(requiredExtensions.begin(), requiredExtensions.end());

        for (const auto& [extensionName, specVersion] : availableExtensions)
            extensionSet.erase(extensionName);
        

        return extensionSet.empty();
    }

    QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices(const VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        
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
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Context::WindowSurface->GetHandle(), &presentSupport);
            if (presentSupport) indices.Present = i;

            // break early if all indices have been found
            if (indices.IsComplete()) break;
            
            i++;
        }
        
        return indices;
    }

    SwapchainSupportDetails PhysicalDevice::QuerySwapchainSupport(const VkPhysicalDevice device) const
    {
        SwapchainSupportDetails details;
        const VkSurfaceKHR windowSurface = Context::WindowSurface->GetHandle();
        
        // Basic surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device == VK_NULL_HANDLE ? m_PhysicalDevice : device, 
            windowSurface, &details.Capabilities);
        
        // Supported surface formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device == VK_NULL_HANDLE ? m_PhysicalDevice : device, 
            windowSurface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device == VK_NULL_HANDLE ? m_PhysicalDevice : device, 
                windowSurface, &formatCount, details.Formats.data());
        }
        
        // Supported presentation modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device == VK_NULL_HANDLE ? m_PhysicalDevice : device, 
            windowSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device == VK_NULL_HANDLE ? m_PhysicalDevice : device, 
                windowSurface, &presentModeCount, details.PresentModes.data());
        }
        
        return details;
    }
}
