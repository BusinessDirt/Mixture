#pragma once

#include "Platform/Vulkan/Base.hpp"
#include "Platform/Vulkan/Descriptor/Layout.hpp"
#include "Platform/Vulkan/Descriptor/Set.hpp"

#include "Mixture/Renderer/RendererInfo.hpp"
#include "Mixture/Renderer/Pipeline.hpp"

namespace Mixture::Vulkan
{
    class GraphicsPipeline final : public Mixture::GraphicsPipeline
    {
    public:
        OPAL_NON_COPIABLE(GraphicsPipeline);

        explicit GraphicsPipeline(const std::string& shaderName);
        ~GraphicsPipeline() override;

        void Bind() const override;
        void PushConstants(const void* pValues) const override;
        void UpdateGlobalUniformBuffer(const void* bufferInfo) const override;
        void UpdateInstanceTexture(const void* imageInfo) const override; 

    private:
        VULKAN_HANDLE(VkPipeline, m_GraphicsPipeline);
        VkPipelineLayout m_PipelineLayout;
        
        VkPushConstantRange m_PushConstant;
        std::vector<Ref<DescriptorSetLayout>> m_SetLayouts;
        
        Scope<DescriptorSet> m_GlobalSet;
        Vector<Scope<DescriptorSet>> m_InstanceSets;
    };
}
