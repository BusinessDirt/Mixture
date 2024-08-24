#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/Shaders/ShaderManager.hpp"
#include "Mixture/Assets/Shaders/ShaderCode.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorBinding.hpp" // TODO: Move this file to 'Mixture' once DirectX is implemented

namespace Mixture
{
    class AssetManager
    {
    public:
        AssetManager();
        ~AssetManager();

        const ShaderCode& GetShader(const std::string& filename) const;
        std::filesystem::path GetTexturePath(const std::string& filename) const;
        std::filesystem::path GetModelPath(const std::string& filename) const;
        std::vector<Vulkan::DescriptorBinding> GetDescriptorBindings() const;

    private:
        std::filesystem::path m_AssetsPath;
        Scope<ShaderManager> m_ShaderManager;
    };
}
