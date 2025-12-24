#pragma once
#include "Platform/Vulkan/Definitions.hpp"

#include "Mixture/Render/RHI/IPipeline.hpp"

namespace Mixture::Vulkan
{
    class Shader : public RHI::IShader
    {
    public:
        Shader(const void* data, size_t size, RHI::ShaderStage stage);
        ~Shader();

        RHI::ShaderStage GetStage() const override { return m_Stage; }

    private:
        RHI::ShaderStage m_Stage;
    };
}
