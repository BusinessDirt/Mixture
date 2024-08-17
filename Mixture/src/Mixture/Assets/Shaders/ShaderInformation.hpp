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

    struct SampledImageInformation
    {
        uint32_t Binding = 0;
        uint32_t Set = 0;
        VkShaderStageFlags Flags = 0;
        
        bool operator==(const SampledImageInformation& other) const
        {
            return Binding == other.Binding && Set == other.Set;
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
        std::vector<const SampledImageInformation*> SampledImages; // these are owned by the shadermanager
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
            std::size_t h1 = std::hash<uint32_t>{}(info.Set);
            std::size_t h2 = std::hash<uint32_t>{}(info.Binding);
            std::size_t h3 = std::hash<uint32_t>{}(info.Size);
            return h1 ^ (h2 << 1) ^ (h3 >> 1);
        }
    };

    template<>
    struct hash<Mixture::SampledImageInformation>
    {
        std::size_t operator()(const Mixture::SampledImageInformation& info) const noexcept
        {
            std::size_t h1 = std::hash<uint32_t>{}(info.Binding);
            std::size_t h2 = std::hash<uint32_t>{}(info.Set);
            
            // Combine the hashes using XOR and bit shifting (example hash combination technique)
            return h1 ^ (h2 << 1);
        }
    };
}
