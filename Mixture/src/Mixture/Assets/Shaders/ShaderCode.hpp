#pragma once

#include "Mixture/Core/Base.hpp"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include <filesystem>
#include <unordered_set>

#include "Mixture/Assets/Shaders/ShaderInformation.hpp"

namespace Mixture
{
    class ShaderCode
    {
    public:
        ShaderCode(const std::filesystem::path& path, const std::filesystem::path& cachePath,
            std::unordered_set<UniformBufferInformation>& uboInfos, std::unordered_set<SampledImageInformation>& images, bool compile);
        ~ShaderCode() = default;

        VkShaderModuleCreateInfo CreateInfo() const;
        const ShaderInformation& GetInformation() const { return m_ShaderInformation; }
        const VkShaderStageFlagBits GetStageFlagBits() const { return m_ShaderStageFlagBits; }

    private:
        void Compile(const std::filesystem::path& path, const std::filesystem::path& cachePath);

        void Reflect(const std::filesystem::path& path, std::unordered_set<UniformBufferInformation>& uboInfos, std::unordered_set<SampledImageInformation>& images);

    private:

        std::vector<uint32_t> m_Data;
        VkShaderStageFlagBits m_ShaderStageFlagBits{};
        ShaderInformation m_ShaderInformation{};
    };
}
