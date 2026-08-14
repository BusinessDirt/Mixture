#pragma once

/**
 * @file IShaderReflector.hpp
 * @brief API-specific shader bytecode reflection.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Render/RHI/RenderFormats.hpp"
#include "Mixture/Render/RHI/ShaderDefinitions.hpp"

#include <slang/slang.h>

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
            uint32_t Binding;
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

    class SlangShaderReflector
    {
    public:
        SlangShaderReflector() = default;
        ~SlangShaderReflector() = default;

        ShaderReflectionData Reflect(slang::ProgramLayout* layout) const;

    private:
        static void ReflectEntryPoints(slang::ProgramLayout& layout, ShaderReflectionData& data);
        static void ReflectResources(slang::ProgramLayout& layout, ShaderReflectionData& data);
        static void ReflectVertexInputs(slang::ProgramLayout& layout, ShaderReflectionData& data);

        static void AddResource(slang::VariableLayoutReflection& variable, ShaderReflectionData& data);
        static void AddPushConstant(slang::ProgramLayout& layout,
            slang::VariableLayoutReflection& variable, ShaderReflectionData& data);
        static void AddVertexInput(slang::VariableLayoutReflection& input, ShaderReflectionData& data);

        static RHI::ShaderStage ToShaderStage(SlangStage stage);
        static RHI::Format ToVertexFormat(slang::TypeReflection& type);
        static uint32_t GetPushConstantStages(slang::ProgramLayout& layout);
    };
}
