#ifdef MX_PLATFORM_MACOSX
#include "Platform/Vulkan/VulkanManager.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Mixture
{
    VulkanManager::VulkanManager()
    {
        GetRequiredLayers();
        GetRequiredInstanceExtensions();
        GetRequiredDeviceExtensions();
    }

    void VulkanManager::Init()
    {
        MX_CORE_ASSERT(CheckLayerSupport(), "Layers requested, but not available!");
        MX_CORE_ASSERT(CheckInstanceExtensionSupport(), "Instance Extensions requested, but not available!");
    }

    bool VulkanManager::CheckLayerSupport()
    {
        for (const char* layerName : m_Layers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : GetAvailableLayers())
            {
                if (strcmp(layerName, layerProperties.layerName) == 0) 
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                MX_CORE_ERROR("Layer not available: {0}", layerName);
                return false;
            }
        }

        return true;
    }

    bool VulkanManager::CheckInstanceExtensionSupport()
    {
        for (const char* extensionName : m_InstanceExtensions)
        {
            bool extensionFound = false;

            for (const auto& extensionProperties : GetAvailableInstanceExtensions())
            {
                if (strcmp(extensionName, extensionProperties.extensionName) == 0) 
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                MX_CORE_ERROR("Instance Extension not available: {0}", extensionName);
                return false;
            }
        }

        return true;
    }

    bool VulkanManager::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
    {
        for (const char* extensionName : m_DeviceExtensions)
        {
            bool extensionFound = false;

            for (const auto& extensionProperties : GetAvailableDeviceExtensions(device))
            {
                if (strcmp(extensionName, extensionProperties.extensionName) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                MX_CORE_ERROR("Device Extension not available: {0}", extensionName);
                return false;
            }
        }

        return true;
    }

    std::vector<VkLayerProperties> VulkanManager::GetAvailableLayers() const
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        /*MX_CORE_INFO("Available Layers: ");
        for (const auto& layer : availableLayers)
        {
            MX_CORE_INFO("    {0}", layer.layerName);
        }*/
            
        
        return availableLayers;
    }

    std::vector<VkExtensionProperties> VulkanManager::GetAvailableInstanceExtensions() const
    {
        uint32_t extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        
        /*MX_CORE_INFO("Available Extensions: ");
        for (const auto& extension : availableExtensions)
        {
            MX_CORE_INFO("    {0}", extension.extensionName);
        }*/
        
        return availableExtensions;
    }

    std::vector<VkExtensionProperties> VulkanManager::GetAvailableDeviceExtensions(VkPhysicalDevice device) const
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        
        /*MX_CORE_INFO("Available Extensions: ");
        for (const auto& extension : availableExtensions)
        {
            MX_CORE_INFO("    {0}", extension.extensionName);
        }*/
        
        return availableExtensions;
    }

    void VulkanManager::GetRequiredLayers()
    {
#ifdef MX_DEBUG
        m_Layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    }

    void VulkanManager::GetRequiredInstanceExtensions()
    {
        // Required for MoltenVK
        m_InstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        
        // glfw extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; ++i) m_InstanceExtensions.push_back(glfwExtensions[i]);
        
#ifdef MX_DEBUG
        m_InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    }

    void VulkanManager::GetRequiredDeviceExtensions()
    {
        // Required for MoltenVK
        m_DeviceExtensions.push_back("VK_KHR_portability_subset");
        
        // swapchain
        m_DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
}

#endif
