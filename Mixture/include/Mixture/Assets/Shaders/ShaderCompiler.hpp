#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/RHI.hpp"

namespace Mixture
{
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

        enum class AttributeFormat
        {
            Float, Vec2, Vec3, Vec4,
            Int, IVec2, IVec3, IVec4,
            UInt, UVec2, UVec3, UVec4,
            Mat3, Mat4,
            Unknown
        };

        struct VertexAttribute
        {
            std::string Name;
            uint32_t Location;
            AttributeFormat Format;
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

    class ShaderCompiler
    {
    public:
        static Vector<uint8_t> Compile(const std::string& source);
        static Vector<uint8_t> ConvertToMSL(const Vector<uint8_t>& spv);

        static ShaderReflectionData ReflectSPIRV(const void* binaryData, size_t binarySize);
        static ShaderReflectionData ReflectDXIL(const void* binaryData, size_t binarySize);
    };
}
