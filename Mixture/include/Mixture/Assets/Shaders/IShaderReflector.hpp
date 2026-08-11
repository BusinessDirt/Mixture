#pragma once

/**
 * @file IShaderReflector.hpp
 * @brief API-specific shader bytecode reflection.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RHI.hpp"

namespace Mixture
{
    /** @brief Reflection data extracted from compiled shader bytecode. */
    struct ShaderReflectionData
    {
        enum class ResourceType
        {
            UniformBuffer,
            StorageBuffer,
            SampledImage,
            StorageImage,
            Sampler,
            InputAttachment,
            Unknown
        };

        struct Resource
        {
            std::string Name;
            ResourceType Type;
            uint32_t Set;
            uint32_t Binding;
            uint32_t Size;
            uint32_t Count;
        };

        struct PushConstant
        {
            std::string Name;
            uint32_t Offset;
            uint32_t Size;
            uint32_t ShaderStage;
        };

        struct VertexAttribute
        {
            std::string Name;
            uint32_t Location;
            RHI::Format PixelFormat;
            uint32_t Size;
            uint32_t Offset;
        };

        std::map<RHI::ShaderStage, std::string> EntryPoints;
        Vector<Resource> UniformBuffers;
        Vector<Resource> StorageBuffers;
        Vector<Resource> Textures;
        Vector<Resource> StorageImages;
        Vector<Resource> Samplers;
        Vector<PushConstant> PushConstants;
        Vector<VertexAttribute> InputAttributes;
    };

    /** @brief Reflects the bytecode format used by one graphics API. */
    class IShaderReflector
    {
    public:
        virtual ~IShaderReflector() = default;

        virtual ShaderReflectionData Reflect(const void* binaryData, size_t binarySize) const = 0;

        /**
         * @brief Creates a reflector for bytecode produced for @p graphicsAPI.
         * @return A reflector, or nullptr when that API has no reflection backend.
         */
        static Scope<IShaderReflector> Create(RHI::GraphicsAPI graphicsAPI);
    };
}
