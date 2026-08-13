#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Command/CommandList.hpp"
#include "Platform/Metal/Pipeline/Pipeline.hpp"
#include "Platform/Metal/Resources/Buffer.hpp"
#include "Platform/Metal/Resources/Texture.hpp"
#include "Platform/Metal/EnumMapper.hpp"

namespace Mixture::Metal
{
    CommandList::CommandList(MTL::CommandQueue* queue)
        : m_Queue(queue)
    {
    }

    CommandList::~CommandList()
    {
    }

    void CommandList::Begin()
    {
        if (m_Queue)
        {
            m_CommandBuffer = m_Queue->commandBuffer();
        }
    }

    void CommandList::End()
    {
        if (m_CommandBuffer)
        {
            m_CommandBuffer->commit();
        }
    }

    void CommandList::BeginRendering(const RHI::RenderingInfo& info)
    {
        if (!m_CommandBuffer) return;

        MTL::RenderPassDescriptor* passDesc = MTL::RenderPassDescriptor::renderPassDescriptor();

        uint32_t i = 0;
        for (const auto& colorAtt : info.ColorAttachments)
        {
            if (colorAtt.Image)
            {
                auto* tex = static_cast<Texture*>(colorAtt.Image);
                MTL::RenderPassColorAttachmentDescriptor* ca = passDesc->colorAttachments()->object(i++);
                ca->setTexture(tex->GetHandle());
                ca->setLoadAction(EnumMapper::MapLoadOp(colorAtt.LoadOp));
                ca->setStoreAction(EnumMapper::MapStoreOp(colorAtt.StoreOp));
                ca->setClearColor(MTL::ClearColor(colorAtt.ClearColor[0], colorAtt.ClearColor[1], colorAtt.ClearColor[2], colorAtt.ClearColor[3]));
            }
        }

        if (info.DepthAttachment && info.DepthAttachment->Image)
        {
            auto* tex = static_cast<Texture*>(info.DepthAttachment->Image);
            MTL::RenderPassDepthAttachmentDescriptor* da = passDesc->depthAttachment();
            da->setTexture(tex->GetHandle());
            da->setLoadAction(EnumMapper::MapLoadOp(info.DepthAttachment->LoadOp));
            da->setStoreAction(EnumMapper::MapStoreOp(info.DepthAttachment->StoreOp));
            da->setClearDepth(info.DepthAttachment->DepthClearValue);
        }

        m_Encoder = m_CommandBuffer->renderCommandEncoder(passDesc);
    }

    void CommandList::EndRendering()
    {
        if (m_Encoder)
        {
            m_Encoder->endEncoding();
            m_Encoder = nullptr;
        }
    }

    void CommandList::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        if (m_Encoder)
        {
            MTL::Viewport vp{ x, y, width, height, minDepth, maxDepth };
            m_Encoder->setViewport(vp);
        }
    }

    void CommandList::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        if (m_Encoder)
        {
            MTL::ScissorRect rect{ static_cast<NS::UInteger>(x > 0 ? x : 0), static_cast<NS::UInteger>(y > 0 ? y : 0), width, height };
            m_Encoder->setScissorRect(rect);
        }
    }

    void CommandList::BindPipeline(RHI::IPipeline* pipeline)
    {
        if (m_Encoder && pipeline)
        {
            auto* mtlPipeline = static_cast<Pipeline*>(pipeline);
            m_Encoder->setRenderPipelineState(mtlPipeline->GetPipelineState());
            if (mtlPipeline->GetDepthStencilState())
            {
                m_Encoder->setDepthStencilState(mtlPipeline->GetDepthStencilState());
            }
        }
    }

    void CommandList::BindVertexBuffer(RHI::IBuffer* buffer, uint32_t binding)
    {
        if (m_Encoder && buffer)
        {
            auto* mtlBuf = static_cast<Buffer*>(buffer);
            m_Encoder->setVertexBuffer(mtlBuf->GetHandle(), 0, binding);
        }
    }

    void CommandList::BindIndexBuffer(RHI::IBuffer* buffer)
    {
        if (buffer)
        {
            auto* mtlBuf = static_cast<Buffer*>(buffer);
            m_IndexBuffer = mtlBuf->GetHandle();
        }
    }

    void CommandList::PushConstants(RHI::IPipeline* pipeline, RHI::ShaderStage stage, const void* data, uint32_t size)
    {
        if (m_Encoder && data && size > 0)
        {
            if (stage == RHI::ShaderStage::Vertex)
                m_Encoder->setVertexBytes(data, size, 30);
            else if (stage == RHI::ShaderStage::Fragment)
                m_Encoder->setFragmentBytes(data, size, 30);
        }
    }

    void CommandList::SetUniformBuffer(uint32_t binding, RHI::IBuffer* buffer, uint32_t set)
    {
        if (m_Encoder && buffer)
        {
            auto* mtlBuf = static_cast<Buffer*>(buffer);
            m_Encoder->setVertexBuffer(mtlBuf->GetHandle(), 0, binding);
            m_Encoder->setFragmentBuffer(mtlBuf->GetHandle(), 0, binding);
        }
    }

    void CommandList::SetTexture(uint32_t binding, RHI::ITexture* texture, uint32_t set)
    {
        if (m_Encoder && texture)
        {
            auto* mtlTex = static_cast<Texture*>(texture);
            m_Encoder->setFragmentTexture(mtlTex->GetHandle(), binding);
        }
    }

    void CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        if (m_Encoder)
        {
            m_Encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, firstVertex, vertexCount, instanceCount, firstInstance);
        }
    }

    void CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
        if (m_Encoder && m_IndexBuffer)
        {
            m_Encoder->drawIndexedPrimitives(
                MTL::PrimitiveTypeTriangle,
                indexCount,
                MTL::IndexTypeUInt32,
                m_IndexBuffer,
                firstIndex * sizeof(uint32_t),
                instanceCount,
                vertexOffset,
                firstInstance
            );
        }
    }
}
#endif
