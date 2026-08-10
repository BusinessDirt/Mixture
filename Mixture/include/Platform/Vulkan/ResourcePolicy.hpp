#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"

namespace Mixture::Vulkan
{
    struct ResourceStateMapping
    {
        vk::PipelineStageFlags Stages;
        vk::AccessFlags Access;
        vk::ImageLayout Layout;
    };

    inline ResourceStateMapping MapResourceState(RHI::ResourceState state)
    {
        switch (state)
        {
            case RHI::ResourceState::VertexAndConstantBuffer:
                return { vk::PipelineStageFlagBits::eVertexInput | vk::PipelineStageFlagBits::eVertexShader,
                    vk::AccessFlagBits::eVertexAttributeRead | vk::AccessFlagBits::eUniformRead, vk::ImageLayout::eGeneral };
            case RHI::ResourceState::IndexBuffer:
                return { vk::PipelineStageFlagBits::eVertexInput, vk::AccessFlagBits::eIndexRead, vk::ImageLayout::eGeneral };
            case RHI::ResourceState::RenderTarget:
                return { vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
                    vk::ImageLayout::eColorAttachmentOptimal };
            case RHI::ResourceState::UnorderedAccess:
                return { vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader,
                    vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite, vk::ImageLayout::eGeneral };
            case RHI::ResourceState::DepthStencilWrite:
                return { vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                    vk::ImageLayout::eDepthStencilAttachmentOptimal };
            case RHI::ResourceState::DepthStencilRead:
                return { vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eDepthStencilAttachmentRead, vk::ImageLayout::eDepthStencilReadOnlyOptimal };
            case RHI::ResourceState::ShaderResource:
                return { vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader,
                    vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eShaderReadOnlyOptimal };
            case RHI::ResourceState::CopySource:
                return { vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferRead, vk::ImageLayout::eTransferSrcOptimal };
            case RHI::ResourceState::CopyDest:
                return { vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eTransferDstOptimal };
            case RHI::ResourceState::Present:
                return { vk::PipelineStageFlagBits::eBottomOfPipe, {}, vk::ImageLayout::ePresentSrcKHR };
            default:
                return { vk::PipelineStageFlagBits::eTopOfPipe, {}, vk::ImageLayout::eUndefined };
        }
    }

    inline vk::ImageAspectFlags GetImageAspect(RHI::Format format)
    {
        if (format == RHI::Format::D24_UNORM_S8_UINT || format == RHI::Format::D32_FLOAT_S8_UINT)
            return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
        if (format == RHI::Format::D32_FLOAT) return vk::ImageAspectFlagBits::eDepth;
        return vk::ImageAspectFlagBits::eColor;
    }

    inline vk::ImageUsageFlags MapTextureUsage(RHI::TextureUsage usage)
    {
        vk::ImageUsageFlags result;
        if (RHI::HasUsage(usage, RHI::TextureUsage::Sampled)) result |= vk::ImageUsageFlagBits::eSampled;
        if (RHI::HasUsage(usage, RHI::TextureUsage::Storage)) result |= vk::ImageUsageFlagBits::eStorage;
        if (RHI::HasUsage(usage, RHI::TextureUsage::ColorAttachment)) result |= vk::ImageUsageFlagBits::eColorAttachment;
        if (RHI::HasUsage(usage, RHI::TextureUsage::DepthStencilAttachment)) result |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        if (RHI::HasUsage(usage, RHI::TextureUsage::TransferSource)) result |= vk::ImageUsageFlagBits::eTransferSrc;
        if (RHI::HasUsage(usage, RHI::TextureUsage::TransferDestination)) result |= vk::ImageUsageFlagBits::eTransferDst;
        return result;
    }
}
