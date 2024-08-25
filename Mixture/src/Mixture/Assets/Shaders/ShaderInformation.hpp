#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    // Values from VkShaderStageFlagBits
    enum ShaderStage
    {
        SHADER_STAGE_VERTEX = 0x00000001,
        SHADER_STAGE_FRAGMENT = 0x00000010
    };

    struct PushConstant
    {
        uint32_t Size = 0;
        uint32_t Offset = 0;
    };

    struct VertexInputBinding
    {
        uint32_t Binding = 0;
        uint32_t Stride = 0;
        //VkVertexInputRate InputRate;
    };

    struct VertexInputAttribute
    {
        uint32_t Location = 0;
        uint32_t Binding = 0;
        VkFormat Format = VK_FORMAT_UNDEFINED;
        uint32_t Offset = 0;
    };

    /**
    * Contains all the information required by the backend to load the shader and 
    * set all the required pipeline information.
    */
    struct SPVShader
    {
        PushConstant Push{};
        Vector<VertexInputBinding> VertexBindings;
        Vector<VertexInputAttribute> VertexAttributes;
        std::unordered_map<ShaderStage, Vector<uint32_t>> Data;
    };

    namespace Util
    {
        ShaderStage FilePathToShaderStage(const std::filesystem::path& filePath);
    }
}
