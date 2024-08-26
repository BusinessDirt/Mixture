#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"
#include "Mixture/Renderer/Pipeline/GraphicsPipeline.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class PipelineLayout;

    class GraphicsPipeline : public Mixture::GraphicsPipeline
    {
    public:
        MX_NON_COPIABLE(GraphicsPipeline);
        
        GraphicsPipeline(const std::string& shaderName);
        ~GraphicsPipeline() override;
        
        void Bind(const FrameInfo& frameInfo) override;
        void BindInstance(const FrameInfo& frameInfo) override;
        void PushConstants(const FrameInfo& info, const void* pValues) override;
        
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
