#include "mxpch.hpp"
#include "PipelineLayout.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSetLayout.hpp"

namespace Mixture::Vulkan
{
    PipelineLayout::PipelineLayout(VkPushConstantRange* range)
    {
        VkDescriptorSetLayout layout = Context::Get().DescriptorSetManager->GetLayout().GetHandle();
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &layout;
        pipelineLayoutInfo.pushConstantRangeCount = range ? 1 : 0;
        pipelineLayoutInfo.pPushConstantRanges = range;

        MX_VK_ASSERT(vkCreatePipelineLayout(Context::Get().Device->GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout),
            "Failed to create VkPipelineLayout");
    }

    PipelineLayout::~PipelineLayout()
    {
        if (m_PipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(Context::Get().Device->GetHandle(), m_PipelineLayout, nullptr);
            m_PipelineLayout = nullptr;
        }
    }
}
