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
            default: return vk::Format::eUndefined;
        }
    }
}
