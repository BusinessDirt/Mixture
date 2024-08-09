#include "mxpch.hpp"
#include "ShaderManager.hpp"

#include <filesystem>

#include "Mixture/Assets/Shaders/ShaderCode.hpp"

namespace Mixture
{
    namespace Util
    {
        static const std::filesystem::path GetCacheDirectory(const std::filesystem::path& shaderFolder)
        {
            std::filesystem::path cache = shaderFolder / "cache";
            if (!std::filesystem::exists(cache))
                std::filesystem::create_directories(cache);
            return cache;
        }
    }

    ShaderManager::ShaderManager(const std::filesystem::path& sourcePath)
    {
        const std::filesystem::path cachePath = Util::GetCacheDirectory(sourcePath);
        
        for (const auto& sourceFilePath : std::filesystem::directory_iterator(sourcePath))
        {
            if (std::filesystem::is_directory(sourceFilePath)) continue;

            std::string filename = sourceFilePath.path().filename().string();
            std::filesystem::path cachedFilePath = cachePath / (filename + ".spv");
            
            // ignore macos system files
            if (filename == ".DS_Store") continue;

            m_Shaders.insert({ filename, ShaderCode(sourceFilePath, cachedFilePath,
                m_UniformBufferInfos, !std::filesystem::exists(cachedFilePath)) });
        }


        // TODO: recompile if file has been changed
    }

    ShaderManager::~ShaderManager()
    {
    }
}