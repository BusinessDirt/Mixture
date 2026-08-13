#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Shader.hpp
 * @brief Metal implementation of the Shader interface.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/IPipeline.hpp"

namespace Mixture::Metal
{
    class Device;

    /**
     * @brief Metal implementation of a shader module.
     */
    class Shader : public RHI::IShader
    {
    public:
        Shader(Ref<Device> device, const void* data, size_t size, RHI::ShaderStage stage, RHI::ShaderIdentity identity);
        ~Shader();

        RHI::ShaderStage GetStage() const override { return m_Stage; }
        RHI::ShaderIdentity GetIdentity() const override { return m_Identity; }
        bool IsValid() const { return m_Function != nullptr; }

        MTL::Function* GetFunction() const { return m_Function; }
        MTL::Library* GetLibrary() const { return m_Library; }

    private:
        Ref<Device> m_Device;
        RHI::ShaderStage m_Stage;
        RHI::ShaderIdentity m_Identity;
        MTL::Library* m_Library = nullptr;
        MTL::Function* m_Function = nullptr;
    };
}

#endif
