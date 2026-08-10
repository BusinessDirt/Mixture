#include "mxpch.hpp"
#include "Platform/Vulkan/Pipeline/Pipeline.hpp"

#include "Platform/Vulkan/Pipeline/Shader.hpp"

#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    namespace
    {
        vk::DescriptorType MapDescriptorType(ShaderReflectionData::ResourceType type)
        {
            switch (type)
            {
                case ShaderReflectionData::ResourceType::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
                case ShaderReflectionData::ResourceType::StorageBuffer: return vk::DescriptorType::eStorageBuffer;
                case ShaderReflectionData::ResourceType::StorageImage: return vk::DescriptorType::eStorageImage;
                case ShaderReflectionData::ResourceType::Sampler: return vk::DescriptorType::eSampler;
                case ShaderReflectionData::ResourceType::InputAttachment: return vk::DescriptorType::eInputAttachment;
                default: return vk::DescriptorType::eCombinedImageSampler;
            }
        }
    }

    PipelineLayoutDescription BuildPipelineLayoutDescription(
        const Vector<std::pair<const ShaderReflectionData*, RHI::ShaderStage>>& shaders)
    {
        PipelineLayoutDescription result;
        for (const auto& [reflection, stage] : shaders)
        {
            if (!reflection) continue;
            const vk::ShaderStageFlags stageFlags = EnumMapper::MapShaderStage(stage);
            auto addResources = [&](const auto& resources)
            {
                for (const auto& resource : resources)
                {
                    if (result.Sets.size() <= resource.Set) result.Sets.resize(resource.Set + 1);
                    auto& bindings = result.Sets[resource.Set];
                    const auto existing = std::find_if(bindings.begin(), bindings.end(),
                        [&](const auto& binding) { return binding.binding == resource.Binding; });
                    const vk::DescriptorType type = MapDescriptorType(resource.Type);
                    if (existing == bindings.end())
                    {
                        bindings.push_back(vk::DescriptorSetLayoutBinding(
                            resource.Binding, type, std::max(1u, resource.Count), stageFlags));
                    }
                    else
                    {
                        OPAL_ASSERT("Core/Vulkan", existing->descriptorType == type,
                            "Reflected descriptor binding type mismatch across shader stages");
                        existing->stageFlags |= stageFlags;
                        existing->descriptorCount = std::max(existing->descriptorCount, std::max(1u, resource.Count));
                    }
                }
            };
            addResources(reflection->UniformBuffers);
            addResources(reflection->StorageBuffers);
            addResources(reflection->Textures);
            addResources(reflection->StorageImages);
            addResources(reflection->Samplers);

            for (const auto& reflectedRange : reflection->PushConstants)
            {
                const auto existing = std::find_if(result.PushConstants.begin(), result.PushConstants.end(),
                    [&](const auto& range) { return range.offset == reflectedRange.Offset && range.size == reflectedRange.Size; });
                if (existing == result.PushConstants.end())
                    result.PushConstants.push_back(vk::PushConstantRange(stageFlags, reflectedRange.Offset, reflectedRange.Size));
                else
                    existing->stageFlags |= stageFlags;
            }
        }
        for (auto& set : result.Sets)
            std::sort(set.begin(), set.end(), [](const auto& a, const auto& b) { return a.binding < b.binding; });
        return result;
    }

    Pipeline::Pipeline(Ref<Device> device, const RHI::PipelineDesc& desc)
        : m_Device(std::move(device))
    {
        OPAL_ASSERT("Core/Vulkan", m_Device, "Pipeline requires an owning device");
        vk::Device vkDevice = m_Device->GetHandle();

        Vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        auto* vertexShader = static_cast<Shader*>(desc.VertexShader);
        auto* fragmentShader = static_cast<Shader*>(desc.FragmentShader);

        if (!vertexShader)
        {
            OPAL_ERROR("Core/Vulkan", "Vertex Shader is required!");
            return;
        }

        if (&vertexShader->GetDevice() != m_Device.get()
            || (fragmentShader && &fragmentShader->GetDevice() != m_Device.get()))
        {
            OPAL_ERROR("Core/Vulkan", "Pipeline shaders must belong to the pipeline's device!");
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
                vkAttr.format = EnumMapper::MapFormat(attr.PixelFormat);
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
            bool formatSupportsBlend = RHI::IsBlendable(format);
            
            vk::PipelineColorBlendAttachmentState colorBlendAttachment;
            colorBlendAttachment.blendEnable = desc.Blend.enabled && formatSupportsBlend;
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

        Vector<std::pair<const ShaderReflectionData*, RHI::ShaderStage>> reflectedShaders{
            { &vertexShader->GetReflectionData(), RHI::ShaderStage::Vertex }
        };
        if (fragmentShader)
            reflectedShaders.push_back({ &fragmentShader->GetReflectionData(), RHI::ShaderStage::Fragment });
        const auto layoutDescription = BuildPipelineLayoutDescription(reflectedShaders);

        for (const auto& bindings : layoutDescription.Sets)
        {
            vk::DescriptorSetLayoutCreateInfo setInfo({}, bindings);
            m_DescriptorSetLayouts.push_back(vkDevice.createDescriptorSetLayout(setInfo));
        }

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
        pipelineLayoutInfo.setSetLayouts(m_DescriptorSetLayouts);
        m_PushConstantRanges = layoutDescription.PushConstants;
        pipelineLayoutInfo.setPushConstantRanges(m_PushConstantRanges);

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
        vk::Device vkDevice = m_Device->GetHandle();

        if (m_Handle) vkDevice.destroyPipeline(m_Handle);
        if (m_Layout) vkDevice.destroyPipelineLayout(m_Layout);
        for (const auto layout : m_DescriptorSetLayouts) vkDevice.destroyDescriptorSetLayout(layout);
    }

    const vk::PushConstantRange* Pipeline::FindPushConstantRange(vk::ShaderStageFlags stage, uint32_t size) const
    {
        const auto range = std::find_if(m_PushConstantRanges.begin(), m_PushConstantRanges.end(),
            [&](const auto& candidate) { return (candidate.stageFlags & stage) == stage && size <= candidate.size; });
        return range == m_PushConstantRanges.end() ? nullptr : &*range;
    }
}
