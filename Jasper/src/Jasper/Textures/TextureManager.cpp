#include "Jasper/Textures/TextureManager.hpp"

namespace Jasper
{
    TextureManager::TextureManager(Settings settings)
        : m_Settings(std::move(settings))
    {
        
    }

    std::weak_ptr<Texture> TextureManager::Get(const TextureHandle handle)
    {
        std::lock_guard lock(m_Mutex);
        if (const auto it = m_GlobalTextures.find(handle); it != m_GlobalTextures.end()) return it->second;
        if (const auto it = m_SceneTextures.find(handle); it != m_SceneTextures.end()) return it->second;
        return {};
    }

    void TextureManager::Unload(const TextureHandle handle)
    {
        std::lock_guard lock(m_Mutex);

        if (const auto it = m_GlobalTextures.find(handle); it != m_GlobalTextures.end())
        {
            // Remove path -> handle mapping if it matches
            if (const auto pit = m_PathToTextureHandle.find(it->second->GetSpecification().Path);
                pit != m_PathToTextureHandle.end() && pit->second == handle)
            {
                m_PathToTextureHandle.erase(pit);
            }

            OPAL_CORE_INFO("Jasper::TextureManager::Unload() - Unloading global texture!");
            m_GlobalTextures.erase(it);
            return;
        }

        if (const auto it = m_SceneTextures.find(handle); it != m_SceneTextures.end())
        {
            // Remove path -> handle mapping if it matches
            if (const auto pit = m_PathToTextureHandle.find(it->second->GetSpecification().Path);
                pit != m_PathToTextureHandle.end() && pit->second == handle)
            {
                m_PathToTextureHandle.erase(pit);
            }

            OPAL_CORE_INFO("Jasper::TextureManager::Unload() - Unloading global texture!");
            m_SceneTextures.erase(it);
            return;
        }

        OPAL_CORE_INFO("Jasper::TextureManager::Unload() - Texture not found: {}", handle);
    }

    void TextureManager::UnloadScene()
    {
        std::lock_guard lock(m_Mutex);
        OPAL_CORE_INFO("Jasper::TextureManager::UnloadScene() - Unloading all scene textures!");
        m_SceneTextures.clear();
    }

    void TextureManager::UnloadGlobal()
    {
        std::lock_guard lock(m_Mutex);
        OPAL_CORE_INFO("Jasper::TextureManager::UnloadGlobal() - Unloading all global textures!");
        m_GlobalTextures.clear();
    }

    void TextureManager::UnloadAll()
    {
        std::lock_guard lock(m_Mutex);
        OPAL_CORE_INFO("Jasper::TextureManager::UnloadAll() - Unloading all textures!");
        m_GlobalTextures.clear();
        m_SceneTextures.clear();
    }
}
