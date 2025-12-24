#include "mxpch.hpp"
#include "Platform/Vulkan/Pipeline/Shader.hpp"

#include <spirv_reflect.h>

namespace Mixture::Vulkan
{
    Shader::Shader(const void* data, size_t size, RHI::ShaderStage stage)
        : m_Stage(stage)
    {
        OPAL_INFO("Core/Vulkan", "");
    }

    Shader::~Shader()
    {

    }
}
