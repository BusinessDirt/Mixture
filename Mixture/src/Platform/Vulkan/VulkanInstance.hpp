#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanManager;

    class VulkanInstance
    {
    public:
        MX_NON_COPIABLE(VulkanInstance);
        
        VulkanInstance(const std::string& applicationName, const VulkanManager& manager);
        ~VulkanInstance();
        
    private:
        VULKAN_HANDLE(VkInstance, m_Instance);
    };
}
