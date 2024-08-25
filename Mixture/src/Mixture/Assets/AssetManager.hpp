#pragma once

#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Descriptor/DescriptorBinding.hpp" // TODO: Move this file to 'Mixture' once DirectX is implemented

namespace Mixture
{
    class AssetManager
    {
    public:
        AssetManager();
        ~AssetManager() = default;

        std::filesystem::path GetTexturePath(const std::string& filename) const;
        std::filesystem::path GetModelPath(const std::string& filename) const;
        std::filesystem::path GetShaderPath() const;

    private:
        std::filesystem::path m_AssetsPath;
    };
}
