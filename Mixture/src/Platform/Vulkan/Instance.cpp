#include "mxpch.hpp"
#include "Platform/Vulkan/Instance.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/Core/Version.hpp"

#include "Platform/Vulkan/Util.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

#ifndef OPAL_DEBUG
    const bool g_EnableValidationLayers = false;
#else
    const bool g_EnableValidationLayers = true;
#endif

namespace Mixture::Vulkan
{
    Instance::Instance(const ApplicationDescription& appDescription)
    {
        try
        {
            CreateInstance(appDescription);
            SetupDebugMessenger();
        }
        catch (...)
        {
            if (m_Handle) m_Handle.destroy();
            m_Handle = nullptr;
            throw;
        }
    }

    Instance::~Instance()
    {
        if (g_EnableValidationLayers && m_Handle && m_DebugMessenger)
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

        if (m_Handle) m_Handle.destroy();
    }

    bool Instance::HasRequiredExtensions(const Vector<vk::ExtensionProperties>& available,
        const Vector<const char*>& required)
    {
        return std::all_of(required.begin(), required.end(), [&](const char* requiredName) {
            return std::any_of(available.begin(), available.end(), [&](const vk::ExtensionProperties& extension) {
                return std::strcmp(requiredName, extension.extensionName) == 0;
            });
        });
    }

    void Instance::CreateInstance(const ApplicationDescription& appDescription)
    {
        Version version = GetVersion();
        vk::ApplicationInfo appInfo(
            appDescription.Name.data(),                                             // App Name
            ParseVulkanVersion(appDescription.Version).value(),                     // App Version
            "Mixture Engine",                                                       // Engine Name
            VK_MAKE_API_VERSION(0, version.Major, version.Minor, version.Patch),    // Engine Version
            VK_API_VERSION_1_3                                                      // API Version
        );

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        if (!glfwExtensions || glfwExtensionCount == 0)
            throw std::runtime_error("GLFW did not provide required Vulkan instance extensions");
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

        if (g_EnableValidationLayers)
        {
            if (!CheckValidationLayerSupport())
                throw std::runtime_error("Vulkan validation layers were requested but are unavailable");

            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }


        const Vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();
        if (!HasRequiredExtensions(availableExtensions, extensions))
            throw std::runtime_error("A required Vulkan instance extension is unavailable");

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

        // Required by MoltenVK when enumerating portability devices.
#ifdef __APPLE__
        createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

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
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            m_Handle, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            // Convert C++ struct to C struct for the raw function call
            VkDebugUtilsMessengerCreateInfoEXT vkCreateInfo = createInfo;
            vkCreateInfo.pfnUserCallback = DebugCallback;
            VkDebugUtilsMessengerEXT vkMessenger;

            if (func(m_Handle, &vkCreateInfo, nullptr, &vkMessenger) != VK_SUCCESS)
                throw std::runtime_error("Failed to create the Vulkan debug messenger");

            m_DebugMessenger = vkMessenger;
        }
        else
        {
            throw std::runtime_error("vkCreateDebugUtilsMessengerEXT is unavailable");
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            OPAL_ERROR("Core/Vulkan", "[Validation Layer]: {}", pCallbackData->pMessage);
        }

        return VK_FALSE;
    }
}
