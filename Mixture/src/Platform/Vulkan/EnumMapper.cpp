#include "mxpch.hpp"
#include "Platform/Vulkan/EnumMapper.hpp"

namespace Mixture::Vulkan
{
    vk::ShaderStageFlagBits EnumMapper::MapShaderStage(RHI::ShaderStage stage)
    {
        switch(stage)
        {
            case RHI::ShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex;
            case RHI::ShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
            case RHI::ShaderStage::Compute: return vk::ShaderStageFlagBits::eCompute;
        }

        return vk::ShaderStageFlagBits::eAll;
    }

    vk::BufferUsageFlags EnumMapper::MapBufferUsage(RHI::BufferUsage usage)
    {
        vk::BufferUsageFlags flags;

        switch (usage)
        {
            case RHI::BufferUsage::Vertex:   flags |= vk::BufferUsageFlagBits::eVertexBuffer; break;
            case RHI::BufferUsage::Index:    flags |= vk::BufferUsageFlagBits::eIndexBuffer; break;
            case RHI::BufferUsage::Uniform:  flags |= vk::BufferUsageFlagBits::eUniformBuffer; break;
            case RHI::BufferUsage::Storage:  flags |= vk::BufferUsageFlagBits::eStorageBuffer; break;
            case RHI::BufferUsage::TransferSrc: flags |= vk::BufferUsageFlagBits::eTransferSrc; break;
            case RHI::BufferUsage::TransferDst: flags |= vk::BufferUsageFlagBits::eTransferDst; break;
        }

        return flags;
    }

    vk::Format EnumMapper::MapFormat(RHI::Format format)
    {
        switch (format)
        {
            case RHI::Format::R8_UNORM:           return vk::Format::eR8Unorm;
            case RHI::Format::R8G8_UNORM:         return vk::Format::eR8G8Unorm;
            case RHI::Format::R8G8B8_UNORM:       return vk::Format::eR8G8B8Unorm;
            case RHI::Format::R8G8B8A8_UNORM:     return vk::Format::eR8G8B8A8Unorm;
            case RHI::Format::B8G8R8A8_UNORM:     return vk::Format::eB8G8R8A8Unorm;
            case RHI::Format::R16_FLOAT:          return vk::Format::eR16Sfloat;
            case RHI::Format::R16G16_FLOAT:       return vk::Format::eR16G16Sfloat;
            case RHI::Format::R16G16B16_FLOAT:    return vk::Format::eR16G16B16Sfloat;
            case RHI::Format::R16G16B16A16_FLOAT: return vk::Format::eR16G16B16A16Sfloat;
            case RHI::Format::R32_FLOAT:          return vk::Format::eR32Sfloat;
            case RHI::Format::R32G32_FLOAT:       return vk::Format::eR32G32Sfloat;
            case RHI::Format::R32G32B32_FLOAT:    return vk::Format::eR32G32B32Sfloat;
            case RHI::Format::R32G32B32A32_FLOAT: return vk::Format::eR32G32B32A32Sfloat;
        }

        return vk::Format::eUndefined;
    }

    vk::PrimitiveTopology EnumMapper::MapTopology(RHI::PrimitiveTopology topology)
    {
        switch (topology)
        {
            case RHI::PrimitiveTopology::TriangleList: return vk::PrimitiveTopology::eTriangleList;
            case RHI::PrimitiveTopology::TriangleStrip: return vk::PrimitiveTopology::eTriangleStrip;
            case RHI::PrimitiveTopology::LineList: return vk::PrimitiveTopology::eLineList;
            case RHI::PrimitiveTopology::PointList: return vk::PrimitiveTopology::ePointList;
        }

        return vk::PrimitiveTopology::eTriangleList;
    }

     vk::CullModeFlags EnumMapper::MapCullMode(RHI::CullMode mode)
     {
        switch(mode)
        {
            case RHI::CullMode::Back: return vk::CullModeFlagBits::eBack;
            case RHI::CullMode::Front: return vk::CullModeFlagBits::eFront;
            case RHI::CullMode::None: return vk::CullModeFlagBits::eNone;
        }

        return vk::CullModeFlagBits::eNone;
    }

    vk::CompareOp EnumMapper::MapCompareOp(RHI::CompareOp compareOp)
    {
        switch(compareOp)
        {
            case RHI::CompareOp::Never: return vk::CompareOp::eNever;
            case RHI::CompareOp::Less: return vk::CompareOp::eLess;
            case RHI::CompareOp::Equal: return vk::CompareOp::eEqual;
            case RHI::CompareOp::LessOrEqual: return vk::CompareOp::eLessOrEqual;
            case RHI::CompareOp::Greater: return vk::CompareOp::eGreater;
            case RHI::CompareOp::NotEqual: return vk::CompareOp::eNotEqual;
            case RHI::CompareOp::GreaterOrEqual: vk::CompareOp::eGreaterOrEqual;
            case RHI::CompareOp::Always: vk::CompareOp::eAlways;
        }

        return vk::CompareOp::eNever;
    }

    vk::AttachmentLoadOp EnumMapper::MapLoadOp(RHI::LoadOp op)
    {
        switch (op)
        {
            case RHI::LoadOp::Clear: return vk::AttachmentLoadOp::eClear;
            case RHI::LoadOp::Load:  return vk::AttachmentLoadOp::eLoad;
            case RHI::LoadOp::DontCare: return vk::AttachmentLoadOp::eDontCare;
            case RHI::LoadOp::None: return vk::AttachmentLoadOp::eNone;
        }

        return vk::AttachmentLoadOp::eDontCare;
    }

    vk::AttachmentStoreOp EnumMapper::MapStoreOp(RHI::StoreOp op)
    {
        switch (op)
        {
            case RHI::StoreOp::Store: return vk::AttachmentStoreOp::eStore;
            case RHI::StoreOp::DontCare: return vk::AttachmentStoreOp::eDontCare;
            case RHI::StoreOp::None: return vk::AttachmentStoreOp::eNone;
        }

        return vk::AttachmentStoreOp::eStore;
    }

    vk::BlendOp EnumMapper::MapBlendOp(RHI::BlendOp op)
    {
        switch(op)
        {
            case RHI::BlendOp::Add: return vk::BlendOp::eAdd;
            case RHI::BlendOp::Subtract: return vk::BlendOp::eSubtract;
            case RHI::BlendOp::ReverseSubtract: return vk::BlendOp::eReverseSubtract;
            case RHI::BlendOp::Min: return vk::BlendOp::eMin;
            case RHI::BlendOp::Max: return vk::BlendOp::eMax;
        }

        return vk::BlendOp::eAdd;
    }

    vk::BlendFactor EnumMapper::MapBlendFactor(RHI::BlendFactor factor)
    {
        switch (factor)
        {
            case RHI::BlendFactor::Zero: return vk::BlendFactor::eZero;
            case RHI::BlendFactor::One: return vk::BlendFactor::eOne;
            case RHI::BlendFactor::SrcColor: return vk::BlendFactor::eSrcColor;
            case RHI::BlendFactor::InvSrcColor: return vk::BlendFactor::eOneMinusSrcColor;
            case RHI::BlendFactor::SrcAlpha: return vk::BlendFactor::eSrcAlpha;
            case RHI::BlendFactor::InvSrcAlpha: return vk::BlendFactor::eOneMinusSrcAlpha;
            case RHI::BlendFactor::DstAlpha: return vk::BlendFactor::eDstAlpha;
            case RHI::BlendFactor::InvDstAlpha: return vk::BlendFactor::eOneMinusDstAlpha;
            case RHI::BlendFactor::DstColor: return vk::BlendFactor::eDstColor;
            case RHI::BlendFactor::InvDstColor: return vk::BlendFactor::eOneMinusDstColor;
            case RHI::BlendFactor::SrcAlphaSaturate: return vk::BlendFactor::eSrcAlphaSaturate;
        }

        return vk::BlendFactor::eZero;
    }

    vk::PolygonMode EnumMapper::MapPolygonMode(RHI::PolygonMode mode)
    {
        switch(mode)
        {
            case RHI::PolygonMode::Fill: return vk::PolygonMode::eFill;
            case RHI::PolygonMode::Line: return vk::PolygonMode::eLine;
            case RHI::PolygonMode::Point: return vk::PolygonMode::ePoint;
        }

        return vk::PolygonMode::eFill;
    }

    vk::FrontFace EnumMapper::MapFrontFace(RHI::FrontFace face)
    {
        switch(face)
        {
            case RHI::FrontFace::Clockwise: return vk::FrontFace::eClockwise;
            case RHI::FrontFace::CounterClockwise: return vk::FrontFace::eCounterClockwise;
        }

        return vk::FrontFace::eClockwise;
    }

    RHI::Format EnumMapper::MapFormat(vk::Format format)
    {
        switch (format)
        {
            case vk::Format::eR8Unorm: return RHI::Format::R8_UNORM;
            case vk::Format::eR8G8Unorm: return RHI::Format::R8G8_UNORM;
            case vk::Format::eR8G8B8Unorm: return RHI::Format::R8G8B8_UNORM;
            case vk::Format::eR8G8B8A8Unorm: return RHI::Format::R8G8B8A8_UNORM;
            case vk::Format::eB8G8R8A8Unorm: return RHI::Format::B8G8R8A8_UNORM;
            case vk::Format::eR16Sfloat: return RHI::Format::R16_FLOAT;
            case vk::Format::eR16G16Sfloat: return RHI::Format::R16G16_FLOAT;
            case vk::Format::eR16G16B16Sfloat: return RHI::Format::R16G16B16_FLOAT;
            case vk::Format::eR16G16B16A16Sfloat: return RHI::Format::R16G16B16A16_FLOAT;
            case vk::Format::eR32Sfloat: return RHI::Format::R32_FLOAT;
            case vk::Format::eR32G32Sfloat: return RHI::Format::R32G32_FLOAT;
            case vk::Format::eR32G32B32Sfloat: return RHI::Format::R32G32B32_FLOAT;
            case vk::Format::eR32G32B32A32Sfloat: return RHI::Format::R32G32B32A32_FLOAT;
            case vk::Format::eD32Sfloat: return RHI::Format::D32_FLOAT;
            case vk::Format::eD24UnormS8Uint: return RHI::Format::D24_UNORM_S8_UINT;
        }

        return RHI::Format::Undefined;
    }
}
