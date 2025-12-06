#include "mxpch.hpp"
#include "Platform/Vulkan/DebugMessenger.hpp"

#include "Platform/Vulkan/Context.hpp"

#include <Opal/Base.hpp>

namespace Mixture::Vulkan
{

    namespace
    {
        VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
        {
        
            switch(messageSeverity)
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                {
                    OPAL_ERROR("Core", pCallbackData->pMessage);
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                {
                    OPAL_WARN("Core", pCallbackData->pMessage);
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                {
                    OPAL_TRACE("Core", pCallbackData->pMessage);
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                {
                    OPAL_INFO("Core", pCallbackData->pMessage);
                    break;
                }
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                {
                    break;
                }
            }

            return VK_FALSE;
        }
    }
    
    DebugMessenger::DebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateCreateInfo(createInfo);

        const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(  // NOLINT(clang-diagnostic-cast-function-type-strict)
            vkGetInstanceProcAddr(Context::Instance->GetHandle(), "vkCreateDebugUtilsMessengerEXT"));
        if (func)
        {
            func(Context::Instance->GetHandle(), &createInfo, nullptr, &m_DebugMessenger);
        }
        else
        {
            OPAL_ERROR("Core", "Mixture::Vulkan::DebugMessenger::DebugMessenger() - Creation failed!");
        }
    }

    DebugMessenger::~DebugMessenger()
    {
        if (const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(  // NOLINT(clang-diagnostic-cast-function-type-strict)
            Context::Instance->GetHandle(), "vkDestroyDebugUtilsMessengerEXT")))
        {
            func(Context::Instance->GetHandle(), m_DebugMessenger, nullptr);
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
