#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/Shaders/ShaderInformation.hpp"

#include <unordered_set>

namespace Mixture
{
    class ShaderCode;

    class ShaderManager
    {
    public:
        ShaderManager(const std::filesystem::path& sourcePath);
        ~ShaderManager();

    private:
        std::unordered_map<std::string, ShaderCode> m_Shaders;
        std::unordered_set<UniformBufferInformation> m_UniformBufferInfos;
        friend class AssetManager;
    };
}
