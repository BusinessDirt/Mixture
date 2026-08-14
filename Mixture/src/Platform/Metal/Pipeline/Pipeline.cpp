#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Pipeline/Pipeline.hpp"
#include "Platform/Metal/Pipeline/Shader.hpp"
#include "Platform/Metal/Device.hpp"
#include "Platform/Metal/EnumMapper.hpp"

namespace Mixture::Metal
{
    namespace
    {
        MTL::VertexFormat GetMetalVertexFormat(RHI::Format format)
        {
            switch (format)
            {
                case RHI::Format::R32_FLOAT:          return MTL::VertexFormatFloat;
                case RHI::Format::R32G32_FLOAT:       return MTL::VertexFormatFloat2;
                case RHI::Format::R32G32B32_FLOAT:    return MTL::VertexFormatFloat3;
                case RHI::Format::R32G32B32A32_FLOAT: return MTL::VertexFormatFloat4;

                case RHI::Format::R16_FLOAT:          return MTL::VertexFormatHalf;
                case RHI::Format::R16G16_FLOAT:       return MTL::VertexFormatHalf2;
                case RHI::Format::R16G16B16_FLOAT:    return MTL::VertexFormatHalf3;
                case RHI::Format::R16G16B16A16_FLOAT: return MTL::VertexFormatHalf4;

                case RHI::Format::R32_INT:            return MTL::VertexFormatInt;
                case RHI::Format::R32G32B32A32_INT:   return MTL::VertexFormatInt4;

                case RHI::Format::R32_UINT:           return MTL::VertexFormatUInt;

                default:
                    OPAL_WARN("Metal", "Unsupported vertex format mapping!");
                    return MTL::VertexFormatInvalid;
            }
        }

        void MergeResources(Vector<ShaderReflectionData::Resource>& target, const Vector<ShaderReflectionData::Resource>& source)
        {
            for (const auto& res : source)
            {
                const auto it = std::find_if(target.begin(), target.end(),
                    [&](const auto& existing) {
                        return existing.Set == res.Set && existing.Binding == res.Binding;
                    });

                if (it == target.end())
                {
                    target.push_back(res);
                }
                else
                {
                    it->Count = std::max(it->Count, res.Count);
                    if (it->Size == 0) it->Size = res.Size;
                }
            }
        }
    }

    void Pipeline::ReflectPipeline(const RHI::PipelineDesc& desc)
    {
        Vector<std::pair<const ShaderReflectionData*, RHI::ShaderStage>> shaders;

        if (desc.VertexShader)
        {
            auto* vertShader = static_cast<Shader*>(desc.VertexShader);
            if (const auto* refl = vertShader->GetReflectionData())
                shaders.push_back({ refl, RHI::ShaderStage::Vertex });
        }

        if (desc.FragmentShader)
        {
            auto* fragShader = static_cast<Shader*>(desc.FragmentShader);
            if (const auto* refl = fragShader->GetReflectionData())
                shaders.push_back({ refl, RHI::ShaderStage::Fragment });
        }

        for (const auto& [refl, stage] : shaders)
        {
            if (!refl) continue;

            for (const auto& [st, name] : refl->EntryPoints) m_ReflectionData.EntryPoints[st] = name;

            MergeResources(m_ReflectionData.UniformBuffers, refl->UniformBuffers);
            MergeResources(m_ReflectionData.StorageBuffers, refl->StorageBuffers);
            MergeResources(m_ReflectionData.Textures, refl->Textures);
            MergeResources(m_ReflectionData.StorageImages, refl->StorageImages);
            MergeResources(m_ReflectionData.Samplers, refl->Samplers);

            for (const auto& pc : refl->PushConstants)
            {
                const auto it = std::find_if(m_ReflectionData.PushConstants.begin(), m_ReflectionData.PushConstants.end(),
                    [&](const auto& existing) {
                        return existing.Binding == pc.Binding && existing.Offset == pc.Offset && existing.Size == pc.Size;
                    });

                if (it == m_ReflectionData.PushConstants.end())
                {
                    m_ReflectionData.PushConstants.push_back(pc);
                }
                else
                {
                    it->ShaderStage |= pc.ShaderStage;
                }
            }

            if (stage == RHI::ShaderStage::Vertex)
            {
                for (const auto& attr : refl->InputAttributes)
                {
                    const auto it = std::find_if(m_ReflectionData.InputAttributes.begin(), m_ReflectionData.InputAttributes.end(),
                        [&](const auto& existing) { return existing.Location == attr.Location; });
                    if (it == m_ReflectionData.InputAttributes.end())
                    {
                        m_ReflectionData.InputAttributes.push_back(attr);
                    }
                }
            }
        }
    }

    const ShaderReflectionData::PushConstant* Pipeline::FindPushConstant(RHI::ShaderStage stage, uint32_t size) const
    {
        const auto it = std::find_if(m_ReflectionData.PushConstants.begin(), m_ReflectionData.PushConstants.end(),
            [&](const auto& pc) {
                return (pc.ShaderStage & static_cast<uint32_t>(stage)) != 0 && size <= pc.Size;
            });
        return it != m_ReflectionData.PushConstants.end() ? &*it : nullptr;
    }

    const ShaderReflectionData::Resource* Pipeline::FindResource(const std::string& name) const
    {
        auto findIn = [&](const Vector<ShaderReflectionData::Resource>& list) -> const ShaderReflectionData::Resource*
        {
            const auto it = std::find_if(list.begin(), list.end(), [&](const auto& r) { return r.Name == name; });
            return it != list.end() ? &*it : nullptr;
        };

        if (const auto* r = findIn(m_ReflectionData.UniformBuffers)) return r;
        if (const auto* r = findIn(m_ReflectionData.StorageBuffers)) return r;
        if (const auto* r = findIn(m_ReflectionData.Textures)) return r;
        if (const auto* r = findIn(m_ReflectionData.StorageImages)) return r;
        if (const auto* r = findIn(m_ReflectionData.Samplers)) return r;
        return nullptr;
    }

    const ShaderReflectionData::Resource* Pipeline::FindUniformBuffer(uint32_t binding, uint32_t set) const
    {
        const auto it = std::find_if(m_ReflectionData.UniformBuffers.begin(), m_ReflectionData.UniformBuffers.end(),
            [&](const auto& r) { return r.Binding == binding && r.Set == set; });
        return it != m_ReflectionData.UniformBuffers.end() ? &*it : nullptr;
    }

    const ShaderReflectionData::Resource* Pipeline::FindTexture(uint32_t binding, uint32_t set) const
    {
        const auto it = std::find_if(m_ReflectionData.Textures.begin(), m_ReflectionData.Textures.end(),
            [&](const auto& r) { return r.Binding == binding && r.Set == set; });
        return it != m_ReflectionData.Textures.end() ? &*it : nullptr;
    }

    const ShaderReflectionData::Resource* Pipeline::FindSampler(uint32_t binding, uint32_t set) const
    {
        const auto it = std::find_if(m_ReflectionData.Samplers.begin(), m_ReflectionData.Samplers.end(),
            [&](const auto& r) { return r.Binding == binding && r.Set == set; });
        return it != m_ReflectionData.Samplers.end() ? &*it : nullptr;
    }

    const ShaderReflectionData::Resource* Pipeline::FindStorageBuffer(uint32_t binding, uint32_t set) const
    {
        const auto it = std::find_if(m_ReflectionData.StorageBuffers.begin(), m_ReflectionData.StorageBuffers.end(),
            [&](const auto& r) { return r.Binding == binding && r.Set == set; });
        return it != m_ReflectionData.StorageBuffers.end() ? &*it : nullptr;
    }

    Pipeline::Pipeline(Ref<Device> device, const RHI::PipelineDesc& desc)
        : m_Device(device)
    {
        if (!m_Device || !m_Device->GetHandle())
        {
            OPAL_ERROR("Core/Metal", "Cannot create pipeline '{}' without a Metal device.", desc.DebugName);
            return;
        }

        MTL::RenderPipelineDescriptor* pipeDesc = MTL::RenderPipelineDescriptor::alloc()->init();
        MTL::VertexDescriptor* vertexDesc = MTL::VertexDescriptor::alloc()->init();

        if (!desc.VertexShader)
        {
            OPAL_ERROR("Core/Metal", "Pipeline '{}' requires a valid vertex shader.", desc.DebugName);
            pipeDesc->release();
            return;
        }

        ReflectPipeline(desc);

        auto* vertShader = static_cast<Shader*>(desc.VertexShader);
        pipeDesc->setVertexFunction(vertShader->GetFunction());

        uint32_t currentOffset = 0;
        const uint32_t vertexBufferIndex = 20;

        auto sortedAttributes = m_ReflectionData.InputAttributes;
        std::sort(sortedAttributes.begin(), sortedAttributes.end(),
            [](const auto& a, const auto& b) { return a.Location < b.Location; });

        for (const auto& attr : sortedAttributes)
        {
            MTL::VertexFormat mtlFormat = GetMetalVertexFormat(attr.PixelFormat);

            uint32_t offset = (attr.Offset > 0) ? attr.Offset : currentOffset;

            vertexDesc->attributes()->object(attr.Location)->setFormat(mtlFormat);
            vertexDesc->attributes()->object(attr.Location)->setOffset(offset);
            vertexDesc->attributes()->object(attr.Location)->setBufferIndex(vertexBufferIndex);

            currentOffset += attr.Size;
        }

        if (!sortedAttributes.empty())
        {
            vertexDesc->layouts()->object(vertexBufferIndex)->setStride(currentOffset);
            vertexDesc->layouts()->object(vertexBufferIndex)->setStepFunction(MTL::VertexStepFunctionPerVertex);
            vertexDesc->layouts()->object(vertexBufferIndex)->setStepRate(1);
        }

        pipeDesc->setVertexDescriptor(vertexDesc);

        if (desc.FragmentShader)
        {
            auto* fragShader = static_cast<Shader*>(desc.FragmentShader);
            if (!fragShader->IsValid())
            {
                OPAL_ERROR("Core/Metal", "Pipeline '{}' has an invalid fragment shader.", desc.DebugName);
                pipeDesc->release();
                return;
            }
            pipeDesc->setFragmentFunction(fragShader->GetFunction());
        }

        uint32_t i = 0;
        for (const auto& fmt : desc.ColorAttachmentFormats)
        {
            MTL::RenderPipelineColorAttachmentDescriptor* colorAtt = pipeDesc->colorAttachments()->object(i++);
            colorAtt->setPixelFormat(EnumMapper::MapFormat(fmt));
            colorAtt->setBlendingEnabled(desc.Blend.enabled);
            colorAtt->setSourceRGBBlendFactor(EnumMapper::MapBlendFactor(desc.Blend.srcColor));
            colorAtt->setDestinationRGBBlendFactor(EnumMapper::MapBlendFactor(desc.Blend.dstColor));
            colorAtt->setRgbBlendOperation(EnumMapper::MapBlendOp(desc.Blend.colorOp));
            colorAtt->setSourceAlphaBlendFactor(EnumMapper::MapBlendFactor(desc.Blend.srcAlpha));
            colorAtt->setDestinationAlphaBlendFactor(EnumMapper::MapBlendFactor(desc.Blend.dstAlpha));
            colorAtt->setAlphaBlendOperation(EnumMapper::MapBlendOp(desc.Blend.alphaOp));
        }

        if (desc.DepthAttachmentFormat != RHI::Format::Undefined)
        {
            pipeDesc->setDepthAttachmentPixelFormat(EnumMapper::MapFormat(desc.DepthAttachmentFormat));

            MTL::DepthStencilDescriptor* dsDesc = MTL::DepthStencilDescriptor::alloc()->init();
            dsDesc->setDepthCompareFunction(EnumMapper::MapCompareOp(desc.DepthStencil.depthCompareOp));
            dsDesc->setDepthWriteEnabled(desc.DepthStencil.depthWrite);
            m_DepthStencilState = m_Device->GetHandle()->newDepthStencilState(dsDesc);
            dsDesc->release();
        }

        NS::Error* error = nullptr;
        m_PipelineState = m_Device->GetHandle()->newRenderPipelineState(pipeDesc, &error);

        if (!m_PipelineState)
        {
            OPAL_ERROR("Core/Metal", "Pipeline '{}' creation failed: {}", desc.DebugName,
                error ? error->localizedDescription()->utf8String() : "unknown error");
        }
        else OPAL_LOG_DEBUG("Core/Metal", "Created render pipeline '{}'.", desc.DebugName);

        m_CullMode = EnumMapper::MapCullMode(desc.Rasterizer.cullMode);
        m_Winding = EnumMapper::MapFrontFace(desc.Rasterizer.frontFace);

        pipeDesc->release();
    }

    Pipeline::~Pipeline()
    {
        if (m_DepthStencilState) { m_DepthStencilState->release(); m_DepthStencilState = nullptr; }
        if (m_PipelineState) { m_PipelineState->release(); m_PipelineState = nullptr; }
    }
}
#endif
