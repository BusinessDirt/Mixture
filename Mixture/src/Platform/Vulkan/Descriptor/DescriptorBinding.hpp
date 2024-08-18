#pragma once

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    struct DescriptorBinding
    {
        uint32_t Binding = 0; // Slot to which the descriptor will be bound, corresponding to the layout index in the shader.
        uint32_t DescriptorCount = 1; // Number of descriptors to bind
        VkDescriptorType Type; // Type of the bound descriptor(s)
        VkShaderStageFlags Stage = VK_SHADER_STAGE_ALL_GRAPHICS; // Shader stage at which the bound resources will be available
    };
}
