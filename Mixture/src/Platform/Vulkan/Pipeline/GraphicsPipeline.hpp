#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class PipelineLayout;

    class GraphicsPipeline
    {
    public:
        MX_NON_COPIABLE(GraphicsPipeline);
        
        GraphicsPipeline();
        ~GraphicsPipeline();
        
    private:
            struct PushConstantInformation
            {
                uint32_t Size;
                uint32_t Offset;
                VkShaderStageFlags Flags;
            };
        
    private:
        VULKAN_HANDLE(VkPipeline, m_Pipeline);
        Scope<PipelineLayout> m_PipelineLayout;
        PushConstantInformation m_PushConstantInformation;
    };
}
