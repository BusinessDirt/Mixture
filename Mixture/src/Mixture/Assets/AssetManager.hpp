#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/Shaders/ShaderManager.hpp"
#include "Mixture/Assets/Shaders/ShaderCode.hpp"

namespace Mixture
{
    class AssetManager
    {
    public:
        AssetManager();
        ~AssetManager();

        const ShaderCode& GetShader(const std::string& filename) const;

    private:
        std::filesystem::path m_AssetsPath;
        Scope<ShaderManager> m_ShaderManager;
    };
}
