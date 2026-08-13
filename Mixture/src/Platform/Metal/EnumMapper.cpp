#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/EnumMapper.hpp"

namespace Mixture::Metal
{
    MTL::PixelFormat EnumMapper::MapFormat(RHI::Format format)
    {
        switch (format)
        {
            case RHI::Format::R8_UNORM: return MTL::PixelFormatR8Unorm;
            case RHI::Format::R8G8_UNORM: return MTL::PixelFormatRG8Unorm;
            case RHI::Format::R8G8B8A8_UNORM: return MTL::PixelFormatRGBA8Unorm;
            case RHI::Format::B8G8R8A8_UNORM: return MTL::PixelFormatBGRA8Unorm;
            case RHI::Format::R16_FLOAT: return MTL::PixelFormatR16Float;
            case RHI::Format::R16G16_FLOAT: return MTL::PixelFormatRG16Float;
            case RHI::Format::R16G16B16A16_FLOAT: return MTL::PixelFormatRGBA16Float;
            case RHI::Format::R32_FLOAT: return MTL::PixelFormatR32Float;
            case RHI::Format::R32G32_FLOAT: return MTL::PixelFormatRG32Float;
            case RHI::Format::R32G32B32A32_FLOAT: return MTL::PixelFormatRGBA32Float;
            case RHI::Format::R32_INT: return MTL::PixelFormatR32Sint;
            case RHI::Format::R32G32B32A32_INT: return MTL::PixelFormatRGBA32Sint;
            case RHI::Format::R32_UINT: return MTL::PixelFormatR32Uint;
            case RHI::Format::D32_FLOAT: return MTL::PixelFormatDepth32Float;
            case RHI::Format::D24_UNORM_S8_UINT: return MTL::PixelFormatDepth24Unorm_Stencil8;
            case RHI::Format::D32_FLOAT_S8_UINT: return MTL::PixelFormatDepth32Float_Stencil8;
            default: return MTL::PixelFormatInvalid;
        }
    }

    RHI::Format EnumMapper::MapFormat(MTL::PixelFormat format)
    {
        switch (format)
        {
            case MTL::PixelFormatR8Unorm: return RHI::Format::R8_UNORM;
            case MTL::PixelFormatRG8Unorm: return RHI::Format::R8G8_UNORM;
            case MTL::PixelFormatRGBA8Unorm: return RHI::Format::R8G8B8A8_UNORM;
            case MTL::PixelFormatBGRA8Unorm: return RHI::Format::B8G8R8A8_UNORM;
            case MTL::PixelFormatR16Float: return RHI::Format::R16_FLOAT;
            case MTL::PixelFormatRG16Float: return RHI::Format::R16G16_FLOAT;
            case MTL::PixelFormatRGBA16Float: return RHI::Format::R16G16B16A16_FLOAT;
            case MTL::PixelFormatR32Float: return RHI::Format::R32_FLOAT;
            case MTL::PixelFormatRG32Float: return RHI::Format::R32G32_FLOAT;
            case MTL::PixelFormatRGBA32Float: return RHI::Format::R32G32B32A32_FLOAT;
            case MTL::PixelFormatR32Sint: return RHI::Format::R32_INT;
            case MTL::PixelFormatRGBA32Sint: return RHI::Format::R32G32B32A32_INT;
            case MTL::PixelFormatR32Uint: return RHI::Format::R32_UINT;
            case MTL::PixelFormatDepth32Float: return RHI::Format::D32_FLOAT;
            case MTL::PixelFormatDepth24Unorm_Stencil8: return RHI::Format::D24_UNORM_S8_UINT;
            case MTL::PixelFormatDepth32Float_Stencil8: return RHI::Format::D32_FLOAT_S8_UINT;
            default: return RHI::Format::Undefined;
        }
    }

    MTL::PrimitiveType EnumMapper::MapTopology(RHI::PrimitiveTopology topology)
    {
        switch (topology)
        {
            case RHI::PrimitiveTopology::PointList: return MTL::PrimitiveTypePoint;
            case RHI::PrimitiveTopology::LineList: return MTL::PrimitiveTypeLine;
            case RHI::PrimitiveTopology::TriangleList: return MTL::PrimitiveTypeTriangle;
            case RHI::PrimitiveTopology::TriangleStrip: return MTL::PrimitiveTypeTriangleStrip;
            default: return MTL::PrimitiveTypeTriangle;
        }
    }

    MTL::CullMode EnumMapper::MapCullMode(RHI::CullMode mode)
    {
        switch (mode)
        {
            case RHI::CullMode::None: return MTL::CullModeNone;
            case RHI::CullMode::Front: return MTL::CullModeFront;
            case RHI::CullMode::Back: return MTL::CullModeBack;
            default: return MTL::CullModeNone;
        }
    }

    MTL::CompareFunction EnumMapper::MapCompareOp(RHI::CompareOp op)
    {
        switch (op)
        {
            case RHI::CompareOp::Never: return MTL::CompareFunctionNever;
            case RHI::CompareOp::Less: return MTL::CompareFunctionLess;
            case RHI::CompareOp::Equal: return MTL::CompareFunctionEqual;
            case RHI::CompareOp::LessOrEqual: return MTL::CompareFunctionLessEqual;
            case RHI::CompareOp::Greater: return MTL::CompareFunctionGreater;
            case RHI::CompareOp::NotEqual: return MTL::CompareFunctionNotEqual;
            case RHI::CompareOp::GreaterOrEqual: return MTL::CompareFunctionGreaterEqual;
            case RHI::CompareOp::Always: return MTL::CompareFunctionAlways;
            default: return MTL::CompareFunctionAlways;
        }
    }

    MTL::LoadAction EnumMapper::MapLoadOp(RHI::LoadOp op)
    {
        switch (op)
        {
            case RHI::LoadOp::Load: return MTL::LoadActionLoad;
            case RHI::LoadOp::Clear: return MTL::LoadActionClear;
            case RHI::LoadOp::DontCare: return MTL::LoadActionDontCare;
            default: return MTL::LoadActionDontCare;
        }
    }

    MTL::StoreAction EnumMapper::MapStoreOp(RHI::StoreOp op)
    {
        switch (op)
        {
            case RHI::StoreOp::Store: return MTL::StoreActionStore;
            case RHI::StoreOp::DontCare: return MTL::StoreActionDontCare;
            default: return MTL::StoreActionDontCare;
        }
    }

    MTL::BlendOperation EnumMapper::MapBlendOp(RHI::BlendOp op)
    {
        switch (op)
        {
            case RHI::BlendOp::Add: return MTL::BlendOperationAdd;
            case RHI::BlendOp::Subtract: return MTL::BlendOperationSubtract;
            case RHI::BlendOp::ReverseSubtract: return MTL::BlendOperationReverseSubtract;
            case RHI::BlendOp::Min: return MTL::BlendOperationMin;
            case RHI::BlendOp::Max: return MTL::BlendOperationMax;
            default: return MTL::BlendOperationAdd;
        }
    }

    MTL::BlendFactor EnumMapper::MapBlendFactor(RHI::BlendFactor factor)
    {
        switch (factor)
        {
            case RHI::BlendFactor::Zero: return MTL::BlendFactorZero;
            case RHI::BlendFactor::One: return MTL::BlendFactorOne;
            case RHI::BlendFactor::SrcColor: return MTL::BlendFactorSourceColor;
            case RHI::BlendFactor::InvSrcColor: return MTL::BlendFactorOneMinusSourceColor;
            case RHI::BlendFactor::SrcAlpha: return MTL::BlendFactorSourceAlpha;
            case RHI::BlendFactor::InvSrcAlpha: return MTL::BlendFactorOneMinusSourceAlpha;
            case RHI::BlendFactor::DstColor: return MTL::BlendFactorDestinationColor;
            case RHI::BlendFactor::InvDstColor: return MTL::BlendFactorOneMinusDestinationColor;
            case RHI::BlendFactor::DstAlpha: return MTL::BlendFactorDestinationAlpha;
            case RHI::BlendFactor::InvDstAlpha: return MTL::BlendFactorOneMinusDestinationAlpha;
            case RHI::BlendFactor::SrcAlphaSaturate: return MTL::BlendFactorSourceAlphaSaturated;
            default: return MTL::BlendFactorOne;
        }
    }

    MTL::Winding EnumMapper::MapFrontFace(RHI::FrontFace face)
    {
        switch (face)
        {
            case RHI::FrontFace::Clockwise: return MTL::WindingClockwise;
            case RHI::FrontFace::CounterClockwise: return MTL::WindingCounterClockwise;
            default: return MTL::WindingCounterClockwise;
        }
    }
}
#endif
