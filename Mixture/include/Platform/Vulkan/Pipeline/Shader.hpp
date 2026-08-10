#pragma once

/**
 * @file Shader.hpp
 * @brief Vulkan implementation of the Shader interface.
 */

#include "Platform/Vulkan/Definitions.hpp"

#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"

namespace Mixture::Vulkan
{
    class Device;

    /**
     * @brief Vulkan implementation of a shader module.
     */
    class Shader : public RHI::IShader
    {
    public:
        /**
         * @brief Constructs a Vulkan Shader module from bytecode.
         * 
         * @param device Shared ownership of the creating device.
         * @param data Pointer to the shader bytecode.
         * @param size Size of the bytecode in bytes.
         * @param stage The shader stage.
         * @param identity Stable logical identity and code version.
         */
        Shader(Ref<Device> device, const void* data, size_t size, RHI::ShaderStage stage,
            RHI::ShaderIdentity identity);
        ~Shader();

        RHI::ShaderStage GetStage() const override { return m_Stage; }
        RHI::ShaderIdentity GetIdentity() const override { return m_Identity; }
        bool IsValid() const { return static_cast<bool>(m_Handle); }

        /** @brief Gets the device that owns this shader module. */
        Device& GetDevice() const { return *m_Device; }

        /**
         * @brief Creates the Vulkan Shader Stage Create Info structure.
         * 
         * Used during pipeline creation.
         * 
         * @return vk::PipelineShaderStageCreateInfo The create info.
         */
        vk::PipelineShaderStageCreateInfo CreateInfo();

        /**
         * @brief Gets the reflection data extracted from the shader.
         * 
         * @return const ShaderReflectionData& The reflection data.
         */
        const ShaderReflectionData& GetReflectionData() const { return m_ReflectionData; }

    private:
        Ref<Device> m_Device;
        RHI::ShaderStage m_Stage;
        RHI::ShaderIdentity m_Identity;
        ShaderReflectionData m_ReflectionData;
        vk::ShaderModule m_Handle = nullptr;
    };
}
