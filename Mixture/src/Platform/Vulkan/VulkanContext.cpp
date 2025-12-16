#include "Platform/Vulkan/VulkanContext.hpp"

#include <GLFW/glfw3.h>

namespace Mixture {

#ifdef OPAL_DEBUG
    const bool g_EnableValidationLayers = false;
#else
    const bool g_EnableValidationLayers = true;
#endif

    VulkanContext::VulkanContext(const ApplicationDescription& appDescription)
        : RHI::IGraphicsContext(appDescription)
    {
        try
        {
            CreateInstance();
            SetupDebugMessenger();
            OPAL_INFO("Core_Vulkan", "Vulkan Initialized Successfully!");
        }
        catch (vk::SystemError& err)
        {
            OPAL_ERROR("Core_Vulkan", "vk::SystemError: {}", err.what());
            exit(-1);
        }
    }

    VulkanContext::~VulkanContext() {
        if (g_EnableValidationLayers)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                m_Instance,
                "vkDestroyDebugUtilsMessengerEXT"
            );

            if (func != nullptr)
            {
                func(m_Instance, m_DebugMessenger, nullptr);
            }
        }

        m_Instance.destroy();
    }

    void VulkanContext::CreateInstance() {
        vk::ApplicationInfo appInfo(
            "Mixture App",            // App Name
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
                throw std::runtime_error("Validation layers requested, but not available!");
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

        m_Instance = vk::createInstance(createInfo);
    }

    bool VulkanContext::CheckValidationLayerSupport()
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

    // -------------------------------------------------------------------------
    // 1. The Callback Function
    // -------------------------------------------------------------------------
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::DebugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        // Filter out noisy "INFO" messages if you want, or keep them all
        if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            OPAL_ERROR("Core_Vulkan", "[Validation Layer]: {}", pCallbackData->pMessage);
        }
        return VK_FALSE; // Should almost always return VK_FALSE
    }

    // -------------------------------------------------------------------------
    // 2. Setup Logic
    // -------------------------------------------------------------------------
    void VulkanContext::SetupDebugMessenger()
    {
        if (!g_EnableValidationLayers) return;

        // Configuration: What do we want to hear about?
        vk::DebugUtilsMessengerCreateInfoEXT createInfo;
        createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                     vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        createInfo.pfnUserCallback = DebugCallback;

        // ---------------------------------------------------------------------
        // Dynamic Lookup: We must find the function pointer manually
        // because it is an Extension function.
        // ---------------------------------------------------------------------
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            m_Instance,
            "vkCreateDebugUtilsMessengerEXT"
        );

        if (func != nullptr)
        {
            // Convert C++ struct to C struct for the raw function call
            VkDebugUtilsMessengerCreateInfoEXT vkCreateInfo = createInfo;
            VkDebugUtilsMessengerEXT vkMessenger;

            if (func(m_Instance, &vkCreateInfo, nullptr, &vkMessenger) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to set up debug messenger!");
            }
            m_DebugMessenger = vkMessenger;
        }
        else
        {
            OPAL_ERROR("Core_Vulkan", "vkCreateDebugUtilsMessengerEXT not found!");
        }
    }
}
