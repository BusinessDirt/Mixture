#include "mxpch.hpp"
#include "DebugMessenger.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{

    // local callback functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        
        switch(messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                MX_CORE_ERROR(pCallbackData->pMessage);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                MX_CORE_WARN(pCallbackData->pMessage);
                break;
            }
            default: break;
        }

        return VK_FALSE;
    }

    DebugMessenger::DebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        DebugMessenger::PopulateCreateInfo(createInfo);

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context::Get().GetInstance().GetHandle(), "vkCreateDebugUtilsMessengerEXT");
        if (func)
        {
            func(Context::Get().GetInstance().GetHandle(), &createInfo, nullptr, &m_DebugMessenger);
        }
        else
        {
            MX_CORE_ERROR("Failed to set up VkDebugUtilsMessengerEXT");
        }
    }

    DebugMessenger::~DebugMessenger()
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context::Get().GetInstance().GetHandle(), "vkDestroyDebugUtilsMessengerEXT");
        if (func)
        {
            func(Context::Get().GetInstance().GetHandle(), m_DebugMessenger, nullptr);
        }
    }

    void DebugMessenger::PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;  // Optional
    }
}
