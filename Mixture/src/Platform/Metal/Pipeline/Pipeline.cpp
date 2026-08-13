#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Pipeline/Pipeline.hpp"
#include "Platform/Metal/Pipeline/Shader.hpp"
#include "Platform/Metal/Device.hpp"
#include "Platform/Metal/EnumMapper.hpp"

namespace Mixture::Metal
{
    Pipeline::Pipeline(Ref<Device> device, const RHI::PipelineDesc& desc)
        : m_Device(device)
    {
        MTL::RenderPipelineDescriptor* pipeDesc = MTL::RenderPipelineDescriptor::alloc()->init();

        if (desc.VertexShader)
        {
            auto* vertShader = static_cast<Shader*>(desc.VertexShader);
            pipeDesc->setVertexFunction(vertShader->GetFunction());
        }

        if (desc.FragmentShader)
        {
            auto* fragShader = static_cast<Shader*>(desc.FragmentShader);
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
        pipeDesc->release();
    }

    Pipeline::~Pipeline()
    {
        if (m_DepthStencilState) { m_DepthStencilState->release(); m_DepthStencilState = nullptr; }
        if (m_PipelineState) { m_PipelineState->release(); m_PipelineState = nullptr; }
    }
}
#endif
