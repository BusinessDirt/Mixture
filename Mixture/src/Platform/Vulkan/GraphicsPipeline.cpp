#include "mxpch.hpp"
#include "Platform/Vulkan/GraphicsPipeline.hpp"

#include <ranges>

#include "Mixture/Assets/ShaderCompiler.hpp"

#include "Platform/Vulkan/ShaderModule.hpp"
#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    GraphicsPipeline::GraphicsPipeline(const std::string& shaderName)
    {
        const Swapchain& swapchain = *Context::Swapchain;

        ShaderCompiler::Flags flags{};
        flags.PipelineType = ShaderCompiler::Graphics;
        flags.Debug = true;

        SpvShader shader = Mixture::ShaderCompiler::Compile(shaderName, flags);
        ShaderModule vertShader(shader, ShaderStageVertex);
        ShaderModule fragShader(shader, ShaderStageFragment);
        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShader.CreateInfo(), fragShader.CreateInfo() };
        
        // Descriptor Set Layouts
        for (const auto& bindings : shader.DescriptorSetLayoutBindings | std::views::values)
            m_SetLayouts.push_back(CreateRef<DescriptorSetLayout>(bindings));
        
        std::vector<VkDescriptorSetLayout> vkLayouts;
        for (auto& layout : m_SetLayouts) vkLayouts.push_back(layout->GetHandle());

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(shader.VertexInputBindings.size());
        vertexInputInfo.pVertexBindingDescriptions = shader.VertexInputBindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(shader.VertexInputAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions = shader.VertexInputAttributes.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain.GetExtent().width);
        viewport.height = static_cast<float>(swapchain.GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {.x = 0, .y = 0 };
        scissor.extent = swapchain.GetExtent();

        std::vector dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(vkLayouts.size());
        pipelineLayoutInfo.pSetLayouts = vkLayouts.data();

        if (shader.PushConstant.size > 0)
        {
            m_PushConstant = shader.PushConstant;
            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &shader.PushConstant;
        }

        VK_ASSERT(vkCreatePipelineLayout(Context::Device->GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout),
                  "Failed to create Graphics Pipeline Layout!")

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_PipelineLayout;
        pipelineInfo.renderPass = swapchain.GetRenderpass().GetHandle();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VK_ASSERT(vkCreateGraphicsPipelines(Context::Device->GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline),
                  "Failed to create Graphics Pipeline")
        
        if (!m_SetLayouts.empty())
        {
            // Define the global set to be set=0
            m_GlobalSet = CreateScope<DescriptorSet>(Context::DescriptorPool->AllocateGlobalSet(m_SetLayouts[0]));
        }

        if (m_SetLayouts.size() >= 2)
        {
            // Define the instance set to be set=1
            m_InstanceSets.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
            for (uint32_t i = 0; i < m_InstanceSets.size(); i++)
            {
                m_InstanceSets[i] = CreateScope<DescriptorSet>(Context::DescriptorPool->AllocateFrameSet(m_SetLayouts[1], i));
            }
        }
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        if (m_GraphicsPipeline)
        {
            vkDestroyPipeline(Context::Device->GetHandle(), m_GraphicsPipeline, nullptr);
            m_GraphicsPipeline = nullptr;
        }

        if (m_PipelineLayout)
        {
            vkDestroyPipelineLayout(Context::Device->GetHandle(), m_PipelineLayout, nullptr);
            m_PipelineLayout = nullptr;
        }
    }

    void GraphicsPipeline::Bind() const
    {
        vkCmdBindPipeline(Context::CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

        const std::array sets = { m_GlobalSet->GetHandle(), m_InstanceSets[Context::Swapchain->GetCurrentFrameIndex()]->GetHandle() };
        vkCmdBindDescriptorSets(Context::CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0,
                                sets.size(), sets.data(), 0, nullptr);
    }

    void GraphicsPipeline::PushConstants(const void* pValues) const
    {
        if (m_PushConstant.size > 0)
        {
            vkCmdPushConstants(Context::CurrentCommandBuffer, m_PipelineLayout, m_PushConstant.stageFlags,
                m_PushConstant.offset, m_PushConstant.size, pValues);
        }
        else
        {
            OPAL_CORE_ERROR("Shader doesn't use push constant!");
        } 
    }

    void GraphicsPipeline::UpdateGlobalUniformBuffer(const void* bufferInfo) const
    {
        if (!m_GlobalSet->GetHandle())
        {
            OPAL_CORE_ERROR("Global descriptor set not initialized!");
            return;
        }
        
        // Binding 0 is inferred here
        m_GlobalSet->UpdateBuffer(0, static_cast<const VkDescriptorBufferInfo*>(bufferInfo));
    }

    void GraphicsPipeline::UpdateInstanceTexture(const void* imageInfo) const
    {
        const DescriptorSet& currentSet = *m_InstanceSets.at(Context::Swapchain->GetCurrentFrameIndex());
        if (!currentSet.GetHandle())
        {
            OPAL_CORE_ERROR("Instance descriptor set not initialized!");
            return;
        }

        // Binding 0 is inferred here
        currentSet.UpdateImage(0, static_cast<const VkDescriptorImageInfo*>(imageInfo));
    }
}
