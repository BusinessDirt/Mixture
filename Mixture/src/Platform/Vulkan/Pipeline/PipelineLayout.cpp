#include "mxpch.hpp"
#include "PipelineLayout.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSetLayout.hpp"

namespace Mixture::Vulkan
{
    PipelineLayout::PipelineLayout(VkPushConstantRange* range)
    {
        VkDescriptorSetLayout global = Context::Get().GetGlobalDescriptorSet().GetLayout().GetHandle();
        VkDescriptorSetLayout instance = Context::Get().GetInstanceDescriptorSet().GetLayout().GetHandle();
        std::vector<VkDescriptorSetLayout> layouts{ global, instance };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
        pipelineLayoutInfo.pSetLayouts = layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = range ? 1 : 0;
        pipelineLayoutInfo.pPushConstantRanges = range;

        MX_VK_ASSERT(vkCreatePipelineLayout(Context::Get().GetDevice().GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout),
            "Failed to create VkPipelineLayout");
    }

    PipelineLayout::~PipelineLayout()
    {
        if (m_PipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(Context::Get().GetDevice().GetHandle(), m_PipelineLayout, nullptr);
            m_PipelineLayout = nullptr;
        }
    }
}
