#pragma once
#include "Mixture/Core/Base.hpp"
#include <vulkan/vulkan.hpp>

#include "Mixture/Render/RHI/RHI.hpp"

namespace Mixture::Vulkan
{
    class EnumMapper
    {
    public:
        static vk::ShaderStageFlagBits MapShaderStage(RHI::ShaderStage stage);
        static vk::BufferUsageFlags MapBufferUsage(RHI::BufferUsage usage);
        static vk::Format MapFormat(RHI::Format format);
        static vk::PrimitiveTopology MapTopology(RHI::PrimitiveTopology topology);
        static vk::CullModeFlags MapCullMode(RHI::CullMode mode);
        static vk::CompareOp MapCompareOp(RHI::CompareOp op);
        static vk::AttachmentLoadOp MapLoadOp(RHI::LoadOp op);
        static vk::AttachmentStoreOp MapStoreOp(RHI::StoreOp op);
        static vk::BlendOp MapBlendOp(RHI::BlendOp op);
        static vk::BlendFactor MapBlendFactor(RHI::BlendFactor factor);
        static vk::PolygonMode MapPolygonMode(RHI::PolygonMode mode);
        static vk::FrontFace MapFrontFace(RHI::FrontFace face);

        static RHI::Format MapFormat(vk::Format format);
    };
}
