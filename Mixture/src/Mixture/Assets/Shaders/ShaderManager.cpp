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
                m_UniformBufferInfos, m_SampledImages, !std::filesystem::exists(cachedFilePath)) });
        }
        
        MX_CORE_INFO("Found ubos:");
        for (const auto& ubo : m_UniformBufferInfos)
        {
            MX_CORE_INFO("layout(set={0}, binding={1}) with size({2})", ubo.Set, ubo.Binding, ubo.Size);
        }
        
        MX_CORE_INFO("Found sampled images:");
        for (const auto& image : m_SampledImages)
        {
            MX_CORE_INFO("layout(set={0}, binding={1})", image.Set, image.Binding);
        }

        // TODO: recompile if file has been changed
    }

    ShaderManager::~ShaderManager()
    {
    }
}
