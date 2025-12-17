#pragma once
#include "Platform/Vulkan/Definitions.hpp"

namespace Mixture
{
    struct ApplicationDescription;
}

namespace Mixture::Vulkan
{
    class Instance
    {
    public:
        Instance(const ApplicationDescription& appDescription);
        ~Instance();

        vk::Instance GetHandle() const { return m_Handle; }

    private:
        void CreateInstance(const ApplicationDescription& appDescription);
        bool CheckValidationLayerSupport();
        void SetupDebugMessenger();

        // The actual callback function Vulkan will call
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            vk::DebugUtilsMessageTypeFlagsEXT messageType,
            const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

    private:
        vk::Instance m_Handle;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;

        const Vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
    };
}
