#pragma once

#include "Jasper/Shaders/ShaderCompiler.hpp"

namespace Jasper
{
    class ShaderManager
    {
    public:
        struct Settings
        {
            bool Debug = true;
            TargetEnvironment Environment = TargetEnvironment::Vulkan;
            std::filesystem::path AssetDirectory = "assets";
            std::string ShaderDirectoryName = "shaders";
            std::string ShaderCacheDirectoryName = "cache";
        };
    public:
        explicit ShaderManager(const Settings& settings);
        ~ShaderManager() = default;

        const SPVShader& Get(const char* shaderName) const { return m_Shaders.at(shaderName); }

    private:
        void LoadCache(const std::filesystem::path& cacheFile);
        void SaveCache(const std::filesystem::path& cacheFile);

        void WriteSPV(const std::filesystem::path& path, const std::vector<uint32_t>& spv) const;
        std::vector<uint32_t> ReadSPV(const std::filesystem::path& path) const;

    private:
        Settings m_Settings;
        
        std::unique_ptr<ShaderCompiler> m_ShaderCompiler;
        std::unordered_map<std::string, SPVShader> m_Shaders;
        std::unordered_map<std::string, std::size_t> m_ShaderCacheMap;
    };
}
