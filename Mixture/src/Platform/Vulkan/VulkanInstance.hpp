#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanInstance
    {
    public:
        MX_NON_COPIABLE(VulkanInstance);
        
        VulkanInstance(const std::string& applicationName);
        ~VulkanInstance();
        
    private:
        VULKAN_HANDLE(VkInstance, m_Instance);
    };
}
