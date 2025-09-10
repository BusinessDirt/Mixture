#pragma once

#include <Opal/Base.hpp>

#include "Jasper/Textures/Texture.hpp"

namespace Jasper
{
    class TextureManager
    {
    public:
        struct Settings
        {
            std::filesystem::path AssetDirectory = "assets";
            std::string TextureDirectoryName = "textures";
        };
    public:
        explicit TextureManager(Settings settings);
        ~TextureManager() = default;

        template<typename T>
        TextureHandle Load(const std::string& path, bool global = true);

        template<typename T>
        TextureHandle Create(const TextureSpecification& specification, bool global = true);
        
        std::weak_ptr<Texture> Get(TextureHandle handle);
        
        void Unload(TextureHandle handle);
        void UnloadScene();
        void UnloadGlobal();
        void UnloadAll();

    private:
        const Settings m_Settings;
        mutable std::mutex m_Mutex;
        
        std::unordered_map<TextureHandle, std::shared_ptr<Texture>> m_GlobalTextures;
        std::unordered_map<TextureHandle, std::shared_ptr<Texture>> m_SceneTextures;
        std::unordered_map<std::string, TextureHandle> m_PathToTextureHandle;
        TextureHandle m_NextHandle = 0;
    };

    template <typename T>
    TextureHandle TextureManager::Load(const std::string& path, const bool global)
    {
        std::lock_guard lock(m_Mutex);
        OPAL_CORE_INFO("Jasper::TextureManager::Load() - Loading Texture: path={}", path.c_str());

        if (const auto pit = m_PathToTextureHandle.find(path);
            pit != m_PathToTextureHandle.end())
        {
            const TextureHandle handle = pit->second;
            return handle;
        }

        TextureHandle handle = m_NextHandle++;
        const std::filesystem::path fullTexturePath = m_Settings.AssetDirectory / m_Settings.TextureDirectoryName / path;
        auto texture = T::Load(fullTexturePath.string());

        if (global) m_GlobalTextures.emplace(handle, texture);
        else m_SceneTextures.emplace(handle, texture);
        
        m_PathToTextureHandle.emplace(path, handle);
        return handle;
    }

    template <typename T>
    TextureHandle TextureManager::Create(const TextureSpecification& specification, const bool global)
    {
        std::lock_guard lock(m_Mutex);
        OPAL_CORE_INFO("Jasper::TextureManager::Create() - Creating Texture: width={}, height={}, mipLevels={}, format={}",
            specification.Width, specification.Height, specification.MipLevels, Util::ImageFormatToString(specification.Format));

        TextureHandle handle = m_NextHandle++;
        auto texture = T::Create(specification);

        if (global) m_GlobalTextures.emplace(handle, texture);
        else m_SceneTextures.emplace(handle, texture);
        
        return handle;
    }
}
