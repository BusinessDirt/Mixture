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
    };
}
