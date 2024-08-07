#ifdef MX_PLATFORM_MACOSX
#include "Platform/Vulkan/VulkanManager.h"

namespace Mixture
{
    VulkanManager::VulkanManager()
    {
        m_Layers.push_back("VK_LAYER_KHRONOS_validation");
        
        // MoltenVK support
        m_Extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }

    void VulkanManager::Init()
    {
        MX_CORE_ASSERT(CheckLayerSupport(), "Layers requested, but not available!");
        MX_CORE_ASSERT(CheckExtensionSupport(), "Extensions requested, but not available!");
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

            if (!layerFound) return false;
        }

        return true;
    }

    bool VulkanManager::CheckExtensionSupport()
    {
        for (const char* extensionName : m_Extensions)
        {
            bool extensionFound = false;

            for (const auto& extensionProperties : GetAvailableExtensions())
            {
                if (strcmp(extensionName, extensionProperties.extensionName) == 0) 
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound) return false;
        }

        return true;
    }

    std::vector<VkLayerProperties> VulkanManager::GetAvailableLayers() const
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        MX_CORE_INFO("Available Layers: ");
        for (const auto& layer : availableLayers)
        {
            MX_CORE_INFO("    {0}", layer.layerName);
        }
            
        
        return availableLayers;
    }

    std::vector<VkExtensionProperties> VulkanManager::GetAvailableExtensions() const
    {
        uint32_t extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        
        MX_CORE_INFO("Available Extensions: ");
        for (const auto& extension : availableExtensions)
        {
            MX_CORE_INFO("    {0}", extension.extensionName);
        }
        
        return availableExtensions;
    }
}

#endif
