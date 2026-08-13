#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file EnumMapper.hpp
 * @brief Metal implementation for enum conversions.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

namespace Mixture::Metal
{
    class EnumMapper
    {
    public:
        static MTL::PixelFormat MapFormat(RHI::Format format);
        static RHI::Format MapFormat(MTL::PixelFormat format);
        static MTL::PrimitiveType MapTopology(RHI::PrimitiveTopology topology);
        static MTL::CullMode MapCullMode(RHI::CullMode mode);
        static MTL::CompareFunction MapCompareOp(RHI::CompareOp op);
        static MTL::LoadAction MapLoadOp(RHI::LoadOp op);
        static MTL::StoreAction MapStoreOp(RHI::StoreOp op);
        static MTL::BlendOperation MapBlendOp(RHI::BlendOp op);
        static MTL::BlendFactor MapBlendFactor(RHI::BlendFactor factor);
        static MTL::Winding MapFrontFace(RHI::FrontFace face);
    };
}

#endif
