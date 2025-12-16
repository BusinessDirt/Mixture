#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

#include <vulkan/vulkan.hpp>
#include <vector>
#include <iostream>

namespace Mixture
{

    class VulkanContext : public RHI::IGraphicsContext
    {
    public:
        VulkanContext(const ApplicationDescription& appDescription);
        ~VulkanContext();

        RHI::GraphicsAPI GetAPI() const override { return RHI::GraphicsAPI::Vulkan; }

        vk::Instance GetInstance() const { return m_Instance; }

    private:
        void CreateInstance();
        bool CheckValidationLayerSupport();

        // The actual callback function Vulkan will call
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            vk::DebugUtilsMessageTypeFlagsEXT messageType,
            const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

        // Helpers to look up the extension functions
        void SetupDebugMessenger();

    private:
        vk::Instance m_Instance;
        vk::DebugUtilsMessengerEXT m_DebugMessenger;

        const std::vector<const char*> m_ValidationLayers =
        {
            "VK_LAYER_KHRONOS_validation"
        };
    };

}
