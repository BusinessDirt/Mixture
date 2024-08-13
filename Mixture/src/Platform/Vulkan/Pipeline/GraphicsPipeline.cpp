#include "mxpch.hpp"
#include "GraphicsPipeline.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/ShaderModule.hpp"
#include "Platform/Vulkan/Pipeline/PipelineLayout.hpp"

namespace Mixture::Util
{
    static bool ValidatePushConstant(const PushConstantInformation& vertPush, const PushConstantInformation& fragPush)
    {
        // If one size is zero and the other is not, return true
        if ((vertPush.Size && !fragPush.Size) || (!vertPush.Size && fragPush.Size))
            return true;

        // If both sizes are the same, return true
        return vertPush.Size == fragPush.Size;
    }

    static VkDescriptorSetLayoutBinding CreateLayoutBinding(const UniformBufferInformation& ubo, VkShaderStageFlags stageFlags)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = ubo.Binding;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr; // Only relevant for image sampling

        return layoutBinding;
    }
}

namespace Mixture::Vulkan
{

    GraphicsPipeline::GraphicsPipeline(const std::string& shaderName)
    {
        std::string vertexFile(shaderName + ".vert");
        std::string fragmentFile(shaderName + ".frag");
        
        const ShaderCode& vertexShaderCode = Application::Get().GetAssetManager().GetShader(vertexFile);
        const ShaderCode& fragmentShaderCode = Application::Get().GetAssetManager().GetShader(fragmentFile);
        
        // Pipeline Layout
        {
            const Mixture::PushConstantInformation& vertPush = vertexShaderCode.GetInformation().PushConstantInformation;
            const Mixture::PushConstantInformation& fragPush = fragmentShaderCode.GetInformation().PushConstantInformation;
            
            if (vertPush.Size == 0 && fragPush.Size == 0)
            {
                // no push constants in shaders
                m_PipelineLayout = CreateScope<PipelineLayout>(nullptr);
            }
            else
            {
                VkPushConstantRange pushConstantRange{};
                if (Util::ValidatePushConstant(vertPush, fragPush))
                {
                    m_PushConstantInformation.Size = vertPush.Size;
                    m_PushConstantInformation.Offset = vertPush.Offset;
                    // push constant can be available in one shader or both
                    m_PushConstantInformation.Flags = (vertPush.Size > 0 ? VK_SHADER_STAGE_VERTEX_BIT : 0) |
                        (fragPush.Size > 0 ? VK_SHADER_STAGE_FRAGMENT_BIT : 0);

                    pushConstantRange.stageFlags = m_PushConstantInformation.Flags;
                    pushConstantRange.offset = m_PushConstantInformation.Offset;
                    pushConstantRange.size = m_PushConstantInformation.Size;
                }
                else
                {
                    MX_CORE_ERROR("Push constant definitions are not the same in '{0}' and '{1}'",
                        vertexFile, fragmentFile);
                }
                
                m_PipelineLayout = CreateScope<PipelineLayout>(&pushConstantRange);
            }
        }
        
        {
            ShaderModule vertModule(vertexShaderCode);
            ShaderModule fragModule(fragmentShaderCode);
            
            int shaderStageCount = 2;
            VkPipelineShaderStageCreateInfo shaderStages[] = { vertModule.CreateInfo(), fragModule.CreateInfo() };
            
            const std::vector<VkVertexInputBindingDescription>& vertexBindings = vertexShaderCode.GetInformation().Bindings;
            const std::vector<VkVertexInputAttributeDescription>& vertexAttributes = vertexShaderCode.GetInformation().Attributes;
            
            // Vertex inputs
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
            vertexInputInfo.pVertexBindingDescriptions = vertexBindings.data();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
            vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();
            
            // Input Assembly
            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;
            
            // Viewport
            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) Context::Get().SwapChain->GetExtent().width;
            viewport.height = (float) Context::Get().SwapChain->GetExtent().height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            
            // Scissor
            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = Context::Get().SwapChain->GetExtent();
            
            // Dynamic States for viewport and scissor
            std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
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
            
            // Rasterizer
            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // TODO: setting for wireframe
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
            
            // Multisampling
            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional
            
            // Color blending
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
            
            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional
            
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
            
            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = shaderStageCount;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = m_PipelineLayout->GetHandle();
            pipelineInfo.renderPass = Context::Get().SwapChain->GetRenderPass().GetHandle();
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional
            
            MX_VK_ASSERT(vkCreateGraphicsPipelines(Context::Get().Device->GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, 
                &m_Pipeline), "Failed to create VkPipeline (Graphics)");
        }
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        if (m_Pipeline)
        {
            vkDestroyPipeline(Context::Get().Device->GetHandle(), m_Pipeline, nullptr);
            m_Pipeline = nullptr;
        }
        
        m_PipelineLayout = nullptr;
    }

    void GraphicsPipeline::Bind(const FrameInfo& frameInfo)
    {
        vkCmdBindPipeline(frameInfo.CommandBuffer.GetAsVulkanHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
        vkCmdBindDescriptorSets(frameInfo.CommandBuffer.GetAsVulkanHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout->GetHandle(), 0, 1, &frameInfo.DescriptorSet, 0, nullptr);
    }

    void GraphicsPipeline::PushConstants(const FrameInfo& frameInfo, const void* pValues)
    {
        vkCmdPushConstants(frameInfo.CommandBuffer.GetAsVulkanHandle(), m_PipelineLayout->GetHandle(), m_PushConstantInformation.Flags,
            m_PushConstantInformation.Offset, m_PushConstantInformation.Size, pValues);
    }
}
