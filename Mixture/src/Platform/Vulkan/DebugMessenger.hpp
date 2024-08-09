#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class DebugMessenger
    {
    public:
        DebugMessenger();
        ~DebugMessenger();

        static void PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    private:
        VULKAN_HANDLE(VkDebugUtilsMessengerEXT, m_DebugMessenger);
    };
}
