#include "mxpch.hpp"
#include "Platform/Vulkan/Instance.hpp"

#include "Mixture/Core/Application.hpp"

#include <GLFW/glfw3.h>

#ifdef OPAL_DEBUG
    const bool g_EnableValidationLayers = false;
#else
    const bool g_EnableValidationLayers = true;
#endif

namespace Mixture::Vulkan
{
    Instance::Instance(const ApplicationDescription& appDescription)
    {
        CreateInstance(appDescription);
        SetupDebugMessenger();
    }

    Instance::~Instance()
    {
        if (g_EnableValidationLayers)
        {
            // We also need to look up the Destroy function manually!
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                m_Handle,
                "vkDestroyDebugUtilsMessengerEXT"
            );

            if (func != nullptr)
            {
                func(m_Handle, m_DebugMessenger, nullptr);
            }
        }

        m_Handle.destroy();
    }

    void Instance::CreateInstance(const ApplicationDescription& appDescription)
    {
        vk::ApplicationInfo appInfo(
            appDescription.name.data(),            // App Name
            VK_MAKE_VERSION(1, 0, 0), // App Version
            "Mixture Engine",         // Engine Name
            VK_MAKE_VERSION(1, 0, 0), // Engine Version
            VK_API_VERSION_1_3        // API Version
        );

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // Add Portability bit for macOS support
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.push_back("VK_KHR_get_physical_device_properties2"); // Often needed for 1.3 features

        if (g_EnableValidationLayers)
        {
            if (!CheckValidationLayerSupport())
            {
                OPAL_ERROR("Core/Vulkan", "Validation layers requested, but not available!");
                return;
            }

            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (g_EnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
            createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        } else
        {
            createInfo.enabledLayerCount = 0;
        }

        // Flags (Required for MacOS/MoltenVK)
        createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

        m_Handle = vk::createInstance(createInfo);
    }

    bool Instance::CheckValidationLayerSupport()
    {
        // Get all available layers
        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

        // Check if our requested layers exist in the available list
        for (const char* layerName : m_ValidationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
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

    void Instance::SetupDebugMessenger()
    {
        if (!g_EnableValidationLayers) return;

        vk::DebugUtilsMessengerCreateInfoEXT createInfo;
        createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        createInfo.pfnUserCallback = DebugCallback;

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            m_Handle, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            // Convert C++ struct to C struct for the raw function call
            VkDebugUtilsMessengerCreateInfoEXT vkCreateInfo = createInfo;
            VkDebugUtilsMessengerEXT vkMessenger;

            if (func(m_Handle, &vkCreateInfo, nullptr, &vkMessenger) != VK_SUCCESS)
            {
                OPAL_ERROR("Core/Vulkan", "Failed to set up debug messenger!");
                return;
            }

            m_DebugMessenger = vkMessenger;
        }
        else
        {
            OPAL_ERROR("Core/Vulkan", "vkCreateDebugUtilsMessengerEXT not found!");
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            OPAL_ERROR("Core/Vulkan", "[Validation Layer]: {}", pCallbackData->pMessage);
        }

        return VK_FALSE;
    }
}
