#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class PipelineLayout
    {
    public:
        MX_NON_COPIABLE(PipelineLayout);
        
        PipelineLayout(VkPushConstantRange* range);
        ~PipelineLayout();
        
    private:
        VULKAN_HANDLE(VkPipelineLayout, m_PipelineLayout);
    };
}
