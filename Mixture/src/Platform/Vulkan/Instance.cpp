#include "mxpch.hpp"
#include "Instance.hpp"

#include "Platform/Vulkan/Manager.hpp"
#include "Platform/Vulkan/DebugMessenger.hpp"

namespace Mixture::Vulkan
{
    Instance::Instance(const std::string& applicationName, const Manager& manager)
    {
        // Setup Vulkan instance.
        VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Mixture";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        
        VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo{};
        layerSettingsCreateInfo.sType = VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT;
        layerSettingsCreateInfo.settingCount = static_cast<uint32_t>(manager.GetLayerSettings().size());
        layerSettingsCreateInfo.pSettings = manager.GetLayerSettings().data();
        layerSettingsCreateInfo.pNext = nullptr;
        
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        DebugMessenger::PopulateCreateInfo(debugMessengerCreateInfo);
        debugMessengerCreateInfo.pNext = &layerSettingsCreateInfo;

        VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(manager.GetInstanceExtensions().size());
        createInfo.ppEnabledExtensionNames = manager.GetInstanceExtensions().data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(manager.GetLayers().size());
        createInfo.ppEnabledLayerNames = manager.GetLayers().data();
#ifdef MX_PLATFORM_MACOSX
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // required for MoltenVK to work
#endif
        createInfo.pNext = &debugMessengerCreateInfo;

        MX_VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance),
                     "Failed to create VkInstance");
    }

    Instance::~Instance()
    {
        if (m_Instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }
    }
}
