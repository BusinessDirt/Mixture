#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class GraphicsPipeline
    {
    public:
        MX_NON_COPIABLE(GraphicsPipeline);
        
        GraphicsPipeline();
        ~GraphicsPipeline();
        
    private:
        VULKAN_HANDLE(VkPipeline, m_Pipeline);
    };
}
