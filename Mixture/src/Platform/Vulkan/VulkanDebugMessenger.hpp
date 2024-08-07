#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanInstance;

    class VulkanDebugMessenger
    {
    public:
        VulkanDebugMessenger(const VulkanInstance& instance);
        ~VulkanDebugMessenger();

        static void PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    private:
        VULKAN_HANDLE(VkDebugUtilsMessengerEXT, m_DebugMessenger);
        const VulkanInstance& m_Instance;
    };
}
