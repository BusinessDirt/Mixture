#include "mxpch.hpp"
#include "Instance.hpp"

#include "Platform/Vulkan/DebugMessenger.hpp"
#include "Mixture/Util/VulkanUtil.hpp"

namespace Mixture::Vulkan
{
    namespace Util
    {
        static std::vector<VkLayerProperties> GetAvailableLayers()
        {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            return availableLayers;
        }

        static std::vector<VkExtensionProperties> GetAvailableExtensions()
        {
            uint32_t extensionCount;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

            return availableExtensions;
        }
    }

    Instance::Instance(const std::string& applicationName, const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions)
    {
        std::vector<VkLayerProperties> availableLayers = Util::GetAvailableLayers();
        std::vector<VkExtensionProperties> availableExtensions = Util::GetAvailableExtensions();

        Util::PrintDebugAvailability(availableLayers, requiredLayers, [](const VkLayerProperties& layer) { return layer.layerName; }, "Layers");
        Util::PrintDebugAvailability(availableExtensions, requiredExtensions, [](const VkExtensionProperties& extension) { return extension.extensionName; }, "Instance Extensions");

        // Setup Vulkan instance.
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Mixture";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;
        
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        DebugMessenger::PopulateCreateInfo(debugMessengerCreateInfo);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
        createInfo.ppEnabledLayerNames = requiredLayers.data();
#ifdef MX_PLATFORM_MACOSX
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // required for MoltenVK to work
#endif
        createInfo.pNext = &debugMessengerCreateInfo;

        MX_VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Failed to create VkInstance");
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
