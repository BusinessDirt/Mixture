#include "mxpch.hpp"
#include "Platform/Vulkan/Instance.hpp"

#include "Platform/Vulkan/DebugMessenger.hpp"

namespace Mixture::Vulkan
{
    namespace Util
    {
        namespace
        {
            std::vector<VkLayerProperties> GetAvailableLayers()
            {
                uint32_t layerCount;
                vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

                std::vector<VkLayerProperties> availableLayers(layerCount);
                vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
            
                return availableLayers;
            }

            std::vector<VkExtensionProperties> GetAvailableExtensions()
            {
                uint32_t extensionCount;
                vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

                std::vector<VkExtensionProperties> availableExtensions(extensionCount);
                vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
            
                return availableExtensions;
            }
        }
    }
    
    Instance::Instance(const std::string& applicationName, const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions)
    {
        const std::vector<VkLayerProperties> availableLayers = Util::GetAvailableLayers();
        const std::vector<VkExtensionProperties> availableExtensions = Util::GetAvailableExtensions();
        
        // Print Layer Information
        Util::PrintDebugAvailability(availableLayers, requiredLayers, [](const VkLayerProperties& layer) { return layer.layerName; }, "Layers");

        // Print Instance Extension Information
        Util::PrintDebugAvailability(availableExtensions, requiredExtensions, [](const VkExtensionProperties& extension) { return extension.extensionName; }, "Instance Extensions");
        
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Mixture";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;
        
//        VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo{};
//        layerSettingsCreateInfo.sType = VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT;
//        layerSettingsCreateInfo.settingCount = static_cast<uint32_t>(manager.GetLayerSettings().size());
//        layerSettingsCreateInfo.pSettings = manager.GetLayerSettings().data();
//        layerSettingsCreateInfo.pNext = nullptr;
        
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        DebugMessenger::PopulateCreateInfo(debugMessengerCreateInfo);
//        debugMessengerCreateInfo.pNext = &layerSettingsCreateInfo;

        // Instance Creation Info
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
        createInfo.ppEnabledLayerNames = requiredLayers.data();
#ifdef OPAL_PLATFORM_DARWIN
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // required for MoltenVK to work
#endif
        createInfo.pNext = &debugMessengerCreateInfo;

        // Create Vulkan Instance
        VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance),
                  "Mixture::Vulkan::Instance::Instance() - Creation failed!")
    }

    Instance::~Instance()
    {
        if (m_Instance)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }
    }
}
