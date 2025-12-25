#pragma once
#include "Platform/Vulkan/Definitions.hpp"

#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"

namespace Mixture::Vulkan
{
    class Shader : public RHI::IShader
    {
    public:
        Shader(const void* data, size_t size, RHI::ShaderStage stage);
        ~Shader();

        RHI::ShaderStage GetStage() const override { return m_Stage; }

        vk::PipelineShaderStageCreateInfo CreateInfo();
        const ShaderReflectionData& GetReflectionData() const { return m_ReflectionData; }

    private:
        RHI::ShaderStage m_Stage;
        ShaderReflectionData m_ReflectionData;
        vk::ShaderModule m_Handle;
    };
}
