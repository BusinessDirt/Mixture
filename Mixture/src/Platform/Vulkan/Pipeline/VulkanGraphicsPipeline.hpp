#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanGraphicsPipeline
    {
    public:
        MX_NON_COPIABLE(VulkanGraphicsPipeline);
        
        VulkanGraphicsPipeline();
        ~VulkanGraphicsPipeline();
        
    private:
        VULKAN_HANDLE(VkPipeline, m_Pipeline);
    };
}