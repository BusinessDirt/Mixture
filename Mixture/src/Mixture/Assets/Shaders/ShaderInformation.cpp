#include "mxpch.hpp"
#include "ShaderInformation.hpp"

namespace Mixture::Util
{
    ShaderStage FilePathToShaderStage(const std::filesystem::path& filePath)
    {
        std::string extension = filePath.extension().string();
        if (extension == ".vert") return SHADER_STAGE_VERTEX;
        if (extension == ".frag") return SHADER_STAGE_FRAGMENT;

        MX_CORE_ERROR("Unknown shader file extension '{0}'", filePath.string().c_str());
        return SHADER_STAGE_VERTEX;
    }
}