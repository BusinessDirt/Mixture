#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>
#include <variant>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Mixture
{
    enum DescriptorType
    {
        DESCRIPTOR_TYPE_SAMPLER = 0,
        DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
        DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
        DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
        DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
        DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
        DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
        DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
        DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
        DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
        DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
        DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000,
        DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR = 1000150000,
        DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV = 1000165000,
        DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM = 1000440000,
        DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM = 1000440001,
        DESCRIPTOR_TYPE_MUTABLE_EXT = 1000351000,
        DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
        DESCRIPTOR_TYPE_MUTABLE_VALVE = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
        DESCRIPTOR_TYPE_MAX_ENUM = 0x7FFFFFFF
    };

    #define STRINGIFY_CASE(name) case name: return #name

    struct DescriptorBinding
    {
        DescriptorType Type;
        uint32_t Size;
    };

    struct DescriptorLayout
    {
        uint32_t Set;
        std::unordered_map<uint32_t, DescriptorBinding> Elements;
    };

    struct GlobalUniformBuffer
    {
        glm::mat4 ViewProjection;
    };

    std::string DescriptorTypeToString(DescriptorType type);
    Vector<DescriptorLayout> GetDefaultLayouts();
}
