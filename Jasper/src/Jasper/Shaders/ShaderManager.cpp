#include "Jasper/Shaders/ShaderManager.hpp"

#include <spirv_cross/spirv.hpp>

#include "Jasper/Util/Util.hpp"

namespace Jasper
{
    namespace Util
    {
        namespace
        {
            std::string GetShaderNameFromPath(const std::filesystem::path& path)
            {
                std::string name = path.filename().string();
                if (name.ends_with(".glsl")) name = name.substr(0, name.size() - 5);

                if (const size_t dotPos = name.rfind('.'); dotPos != std::string::npos) 
                    name = name.substr(0, dotPos);

                return name;
            }
        }
    }
    
    ShaderManager::ShaderManager(const Settings& settings)
    {
        m_Settings = settings;

        const std::filesystem::path cacheFolder = m_Settings.AssetDirectory / m_Settings.ShaderDirectoryName / m_Settings.ShaderCacheDirectoryName;
        const std::filesystem::path cacheFile = cacheFolder / "Jasper.Shaders.cache";
        std::filesystem::create_directory(cacheFolder);
        LoadCache(cacheFile);

        ShaderCompiler::Flags compilerFlags;
        compilerFlags.Debug = settings.Debug;
        compilerFlags.Environment = settings.Environment;
        m_ShaderCompiler = std::make_unique<ShaderCompiler>(compilerFlags);

        // Get all the .glsl files from the 'shader' folder
        std::vector<std::filesystem::path> glslFiles;
        if (const std::filesystem::path shaderDirectory = m_Settings.AssetDirectory / m_Settings.ShaderDirectoryName;
            std::filesystem::is_directory(shaderDirectory) || !std::filesystem::exists(shaderDirectory))
        {
            for (const auto& entry : std::filesystem::directory_iterator(shaderDirectory))
            {
                if (!entry.is_regular_file()) continue;
                if (entry.path().extension() == ".glsl") glslFiles.push_back(entry.path());
            }
        }

        // Compile shader stage or retrieve the cached file
        for (const auto& glslFile : glslFiles)
        {
            const std::string source = Util::ReadFile(glslFile);
            const std::string shaderName = Util::GetShaderNameFromPath(glslFile);
            const size_t hash = std::hash<std::string>()(source);

            ShaderStageFlagBits stage = Util::FilePathToShaderStage(glslFile);
            SPVShader& spvShader = m_Shaders[shaderName];
            spvShader.Name = shaderName;

            if (m_ShaderCacheMap.contains(glslFile.filename().string()) && m_ShaderCacheMap[glslFile.filename().string()] == hash)
            {
                spvShader.Data[stage] = ReadSPV(cacheFolder / (glslFile.filename().string() + ".spv"));
            }
            else
            {
                spvShader.Data[stage] = m_ShaderCompiler->CompileSPV(source, glslFile, stage);

                WriteSPV(cacheFolder / (glslFile.filename().string() + ".spv"), spvShader.Data[stage]);
                m_ShaderCacheMap[glslFile.filename().string()] = hash;
            }

            m_ShaderCompiler->ReflectSPV(spvShader, stage);
        }

        SaveCache(cacheFile);
    }

    void ShaderManager::LoadCache(const std::filesystem::path& cacheFile)
    {
        std::ifstream in(cacheFile);
        if (!in) return;
        
        std::string line;
        while (std::getline(in, line))
        {
            auto pos = line.find(':');
            if (pos == std::string::npos) continue;

            std::string name = line.substr(0, pos);
            std::string hashStr = line.substr(pos + 1);

            // Check if the cached file actually exists before adding it to the cache map
            if (std::filesystem::path spvFile = cacheFile.parent_path() / (name + ".spv");
                !std::filesystem::exists(spvFile))
            {
                if (m_Settings.Debug) OPAL_CORE_WARN("Jasper::ShaderManager::LoadCache() - Cached SPIR-V file '{}' missing, but '{}' is a cache entry",
                    spvFile.string().c_str(), name);
                continue;
            }

            std::istringstream iss(hashStr);
            iss >> m_ShaderCacheMap[name];
        }

        if (m_Settings.Debug) OPAL_CORE_INFO("Jasper::ShaderManager::LoadCache() - Loaded {} shader files from cache", m_ShaderCacheMap.size());
    }

    void ShaderManager::SaveCache(const std::filesystem::path& cacheFile)
    {
        std::filesystem::path tempFile = cacheFile;
        tempFile += ".tmp";

        {
            std::ofstream out(tempFile, std::ios::trunc);
            OPAL_CORE_ASSERT(out, "Jasper::ShaderManager::SaveCache() - Failed to open temporary cache file for writing!")
            for (const auto& [name, hash] : m_ShaderCacheMap)
                out << name << ": " << hash << "\n";
        }

        std::filesystem::rename(tempFile, cacheFile);
    }

    void ShaderManager::WriteSPV(const std::filesystem::path& path, const std::vector<uint32_t>& spv) const
    {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        OPAL_CORE_ASSERT(out, "Jasper::ShaderManager::WriteSPV() - Failed to open SPIR-V file for writing!")

        out.write(reinterpret_cast<const char*>(spv.data()), static_cast<std::streamsize>(spv.size() * sizeof(uint32_t)));
        if (m_Settings.Debug) OPAL_CORE_INFO("Jasper::ShaderManager::WriteSPV() - {} written to cache", path.filename().string());
    }

    std::vector<uint32_t> ShaderManager::ReadSPV(const std::filesystem::path& path) const
    {
        std::ifstream in(path, std::ios::binary | std::ios::ate);
        OPAL_CORE_ASSERT(in, "Jasper::ShaderManager::ReadSPV() - Failed to open SPIR-V file for reading!")

        const std::streamsize size = in.tellg();
        in.seekg(0, std::ios::beg);

        OPAL_CORE_ASSERT(size % sizeof(uint32_t) == 0, "Jasper::ShaderManager::ReadSPV() - SPIR-V file size is not multiple of 4 bytes!")
        
        std::vector<uint32_t> data(size / sizeof(uint32_t));
        in.read(reinterpret_cast<char*>(data.data()), size);

        if (m_Settings.Debug) OPAL_CORE_INFO("Jasper::ShaderManager::ReadSPV() - {} read from cache", path.filename().string());

        return data;
    }
}
