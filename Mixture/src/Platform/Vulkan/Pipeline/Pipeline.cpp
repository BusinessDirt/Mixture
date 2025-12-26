#include "mxpch.hpp"
#include "Platform/Vulkan/Pipeline/Pipeline.hpp"

#include "Platform/Vulkan/Pipeline/Shader.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    Pipeline::Pipeline(const RHI::PipelineDesc& desc)
    {
        auto& device = Context::Get().GetLogicalDevice();
        vk::Device vkDevice = device.GetHandle();

        Vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        auto* vertexShader = static_cast<Shader*>(desc.VertexShader);
        auto* fragmentShader = static_cast<Shader*>(desc.FragmentShader);

        if (!vertexShader)
        {
            OPAL_ERROR("Core/Vulkan", "Vertex Shader is required!");
            return;
        }

        shaderStages.push_back(vertexShader->CreateInfo());
        if (fragmentShader) shaderStages.push_back(fragmentShader->CreateInfo());

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        Vector<vk::VertexInputBindingDescription> bindingDescriptions;
        Vector<vk::VertexInputAttributeDescription> attributeDescriptions;

        const auto& vertexShaderReflection = vertexShader->GetReflectionData();
        if (!vertexShaderReflection.InputAttributes.empty())
        {
            uint32_t stride = 0;

            // Sort attributes by location to ensure correct order
            auto sortedAttributes = vertexShaderReflection.InputAttributes;
            std::sort(sortedAttributes.begin(), sortedAttributes.end(),
                [](const auto& a, const auto& b) { return a.Location < b.Location; });

            for (const auto& attr : sortedAttributes)
            {
                vk::VertexInputAttributeDescription vkAttr;
                vkAttr.binding = 0; // TODO: Assume 1 buffer for simplicity
                vkAttr.location = attr.Location;
                vkAttr.format = EnumMapper::MapFormat(attr.Format);
                vkAttr.offset = stride; // Auto-calculate offset
                attributeDescriptions.push_back(vkAttr);

                stride += attr.Size;
            }

            vk::VertexInputBindingDescription bindingDesc;
            bindingDesc.binding = 0;
            bindingDesc.stride = stride;
            bindingDesc.inputRate = vk::VertexInputRate::eVertex;
            bindingDescriptions.push_back(bindingDesc);

            vertexInputInfo.setVertexBindingDescriptions(bindingDescriptions);
            vertexInputInfo.setVertexAttributeDescriptions(attributeDescriptions);
        }

        // Input Assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
        inputAssembly.topology = EnumMapper::MapTopology(desc.Topology);
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Dynamic viewport and scissor
        vk::PipelineViewportStateCreateInfo viewportState;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        Vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizer;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = EnumMapper::MapPolygonMode(desc.Rasterizer.polygonMode);
        rasterizer.lineWidth = desc.Rasterizer.lineWidth;
        rasterizer.cullMode = EnumMapper::MapCullMode(desc.Rasterizer.cullMode);
        rasterizer.frontFace = EnumMapper::MapFrontFace(desc.Rasterizer.frontFace);
        rasterizer.depthBiasEnable = VK_FALSE;

        // TODO: add Multisampling support
        vk::PipelineMultisampleStateCreateInfo multisampling;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

        vk::PipelineDepthStencilStateCreateInfo depthStencil;
        depthStencil.depthTestEnable = desc.DepthStencil.depthTest;
        depthStencil.depthWriteEnable = desc.DepthStencil.depthWrite;
        depthStencil.depthCompareOp = EnumMapper::MapCompareOp(desc.DepthStencil.depthCompareOp);
        depthStencil.stencilTestEnable = VK_FALSE; // TODO: Add stencil support if needed

        // Create one blend attachment per render target
        Vector<vk::PipelineColorBlendAttachmentState> blendAttachments;
        for (const auto& format : desc.ColorAttachmentFormats)
        {
            vk::PipelineColorBlendAttachmentState colorBlendAttachment;
            colorBlendAttachment.blendEnable = desc.Blend.enabled;
            colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR
                                                | vk::ColorComponentFlagBits::eG
                                                | vk::ColorComponentFlagBits::eB
                                                | vk::ColorComponentFlagBits::eA;

            if (desc.Blend.enabled)
            {
                colorBlendAttachment.srcColorBlendFactor = EnumMapper::MapBlendFactor(desc.Blend.srcColor);
                colorBlendAttachment.dstColorBlendFactor = EnumMapper::MapBlendFactor(desc.Blend.dstColor);
                colorBlendAttachment.colorBlendOp = EnumMapper::MapBlendOp(desc.Blend.colorOp);
                colorBlendAttachment.srcAlphaBlendFactor = EnumMapper::MapBlendFactor(desc.Blend.srcAlpha);
                colorBlendAttachment.dstAlphaBlendFactor = EnumMapper::MapBlendFactor(desc.Blend.dstAlpha);
                colorBlendAttachment.alphaBlendOp = EnumMapper::MapBlendOp(desc.Blend.alphaOp);
            }

            blendAttachments.push_back(colorBlendAttachment);
        }

        vk::PipelineColorBlendStateCreateInfo colorBlending;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.setAttachments(blendAttachments);

        // TODO: A robust engine would merge reflection data from VS and FS here
        // and create DescriptorSetLayouts. For now, we will assume an empty layout
        // or a PushConstant-only layout for the simple triangle case.

        // If you have a DescriptorLayoutCache, use it here.
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo;

        // Add push constants from reflection
        Vector<vk::PushConstantRange> pushConstants;
        // TODO: iterate vsReflection.PushConstants and convert to vk::PushConstantRange ...

        pipelineLayoutInfo.setPushConstantRanges(pushConstants);
        pipelineLayoutInfo.setLayoutCount = 0; // Set to actual descriptor set count

        try
        {
            m_Layout = vkDevice.createPipelineLayout(pipelineLayoutInfo);
        }
        catch (...)
        {
            OPAL_ERROR("Core/Vulkan", "Failed to create Pipeline Layout!");
        }

        vk::PipelineRenderingCreateInfo renderingInfo;
        Vector<vk::Format> colorFormats;
        for(auto f : desc.ColorAttachmentFormats) colorFormats.push_back(EnumMapper::MapFormat(f));

        renderingInfo.setColorAttachmentFormats(colorFormats);
        if (desc.DepthAttachmentFormat != RHI::Format::Undefined)
            renderingInfo.depthAttachmentFormat = EnumMapper::MapFormat(desc.DepthAttachmentFormat);

        vk::GraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = m_Layout;
        pipelineInfo.renderPass = nullptr;
        pipelineInfo.pNext = &renderingInfo;

        auto result = vkDevice.createGraphicsPipeline(nullptr, pipelineInfo);

        if (result.result != vk::Result::eSuccess)
        {
            OPAL_ERROR("Core/Vulkan", "Failed to create Graphics Pipeline!");
        }
        else
        {
            m_Handle = result.value;
        }
    }

    Pipeline::~Pipeline()
    {
        auto& device = Context::Get().GetLogicalDevice();
        vk::Device vkDevice = device.GetHandle();

        if (m_Handle) vkDevice.destroyPipeline(m_Handle);
        if (m_Layout) vkDevice.destroyPipelineLayout(m_Layout);
    }
}
