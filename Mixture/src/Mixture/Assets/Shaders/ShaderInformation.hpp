#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>

namespace Mixture
{
    struct UniformBufferInformation
    {
        uint32_t Size = 0;
        uint32_t Binding = 0;
        uint32_t Set = 0;
        VkShaderStageFlags Flags = 0;
        
        bool operator==(const UniformBufferInformation& other) const
        {
            return Size == other.Size && Binding == other.Binding && Set == other.Set;
        }
    };

    struct PushConstantInformation
    {
        uint32_t Size = 0;
        uint32_t Offset = 0;
    };

    struct ShaderInformation
    {
        PushConstantInformation PushConstantInformation{};
        std::vector<const UniformBufferInformation*> UniformBuffers; // these are owned by the shadermanager
        std::vector<VkVertexInputBindingDescription> Bindings;
        std::vector<VkVertexInputAttributeDescription> Attributes;
    };
}

namespace std 
{
    template<>
    struct hash<Mixture::UniformBufferInformation>
    {
        std::size_t operator()(const Mixture::UniformBufferInformation& info) const
        {
            return ((std::hash<uint32_t>()(info.Size)
                ^ (std::hash<uint32_t>()(info.Binding) << 1)) >> 1)
                ^ (std::hash<uint32_t>()(info.Set) >> 1) << 1;
        }
    };
}
