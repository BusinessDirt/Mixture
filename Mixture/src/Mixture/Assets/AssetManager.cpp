#include "mxpch.hpp"
#include "Mixture/Assets/AssetManager.hpp"

#include "Mixture/Assets/Shaders/ShaderLoader.hpp"

namespace Mixture
{
    void AssetManager::Init()
    {
        m_Loaders[AssetType::Shader] = CreateScope<ShaderLoader>();

        m_AssetCache.SetEvictionCallback([](const UUID& id, const Ref<IAsset>& asset)
        {
            OPAL_LOG_DEBUG("Core/Assets", "Evicting Asset '{}' (ID: {}) Size: {} bytes", 
                asset ? asset->GetName() : "Unknown", 
                (uint64_t)id, 
                asset ? asset->GetMemoryUsage() : 0);
        });
    }

    void AssetManager::SetAssetRoot(const std::filesystem::path& rootPath)
    {
        m_RootDirectory = std::filesystem::absolute(rootPath);
        OPAL_INFO("Core/Assets", "Asset Directory set to: {}", m_RootDirectory.string());

        if (!std::filesystem::exists(m_RootDirectory))
        {
            OPAL_WARN("Core/Assets", "Asset Directory does not exist!");
        }
    }

    AssetHandle AssetManager::GetAsset(AssetType type, const std::filesystem::path& path)
    {
        // Create the UUID deterministically from the path
        // TODO: replace with metadata files
        UUID id = UUID::FromPath(path.string());

        // Check if already loaded in Cache
        Ref<IAsset> cachedAsset = m_AssetCache.Get(id);
        if (cachedAsset) return AssetHandle{ id, cachedAsset->GetMagic() };

        // If not loaded, load it now
        Ref<IAsset> newAsset = LoadAssetInternal(type, path, id);
        if (newAsset) return AssetHandle{ id, newAsset->GetMagic() };

        return AssetHandle{ UUID(0), 0 }; // Return Invalid Handle
    }

    Ref<IAsset> AssetManager::LoadAssetInternal(AssetType type, const std::filesystem::path& path, UUID id)
    {
        const char* typeString = Utils::AssetTypeToString(type);
        if (m_Loaders.find(type) == m_Loaders.end())
        {
            OPAL_ERROR("Core/Assets", "No loader registered for AssetType='{}'", typeString);
            return nullptr;
        }

        AssetMetadata metadata;
        metadata.ID = id;
        metadata.Type = type;
        metadata.FilePath = path;

        FileStreamReader stream(m_RootDirectory / typeString / path);
        if (!stream.IsOpen())
        {
            OPAL_ERROR("Core/Asses", "Failed to open file {}", path.string());
            return nullptr;
        }
            
        // Reset Scratch Arena for this load operation
        // Note: If loading becomes multi-threaded, this needs to be thread-local or locked.
        m_LoadingArena.Reset();

        IAssetLoader& loader = *m_Loaders[type];
        Ref<IAsset> asset = loader.LoadSync(stream, metadata, &m_LoadingArena);
        OPAL_LOG_DEBUG("Core/Assets", "Loaded {} from '{}' with id={}",
            typeString, path.string(), (uint64_t)id);

        if (asset)
        {
            // Assign Magic Number (Generation)
            static std::atomic<uint32_t> s_AssetMagicCounter(1);
            asset->SetMagic(s_AssetMagicCounter++);
            
            // Add to LRU Cache
            m_AssetCache.Put(id, asset, asset->GetMemoryUsage());
        }
        return asset;
    }
}
