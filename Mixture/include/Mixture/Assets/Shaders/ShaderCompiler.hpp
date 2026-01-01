#pragma once

/**
 * @file ShaderCompiler.hpp
 * @brief Utilities for compiling and reflecting shader code.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/RHI/RHI.hpp"

namespace Mixture
{
    /**
     * @brief Contains reflection data extracted from a compiled shader.
     */
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

    /**
     * @brief Static class for compiling shader source code.
     */
    class ShaderCompiler
    {
    public:
        /**
         * @brief Compiles shader source code into SPIR-V.
         *
         * @param source The shader source code.
         * @return Vector<uint8_t> The compiled SPIR-V bytecode.
         */
        static Vector<uint8_t> Compile(const std::string& source);

        /**
         * @brief Converts SPIR-V bytecode to Metal Shading Language (MSL).
         *
         * @param spv The SPIR-V bytecode.
         * @return Vector<uint8_t> The MSL source code.
         */
        static Vector<uint8_t> ConvertToMSL(const Vector<uint8_t>& spv);

        /**
         * @brief Reflects SPIR-V bytecode to extract resources and attributes.
         *
         * @param binaryData Pointer to the SPIR-V data.
         * @param binarySize Size of the data in bytes.
         * @return ShaderReflectionData The extracted reflection data.
         */
        static ShaderReflectionData ReflectSPIRV(const void* binaryData, size_t binarySize);

        /**
         * @brief Reflects DXIL bytecode to extract resources and attributes.
         *
         * @param binaryData Pointer to the DXIL data.
         * @param binarySize Size of the data in bytes.
         * @return ShaderReflectionData The extracted reflection data.
         */
        static ShaderReflectionData ReflectDXIL(const void* binaryData, size_t binarySize);
    };
}
