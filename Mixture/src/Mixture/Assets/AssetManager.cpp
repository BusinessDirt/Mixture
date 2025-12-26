#include "mxpch.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"

#include "Mixture/Assets/Shaders/ShaderSerializer.hpp"

namespace Mixture
{
    void AssetManager::Init()
    {
        m_Serializers[AssetType::Shader] = CreateScope<ShaderSerializer>();

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
        const char* typeString = Utils::AssetTypeToString(type);
        std::filesystem::path fullPath = m_RootDirectory / typeString / path;

        if (!std::filesystem::exists(fullPath))
        {
            OPAL_ERROR("Core/Assets", "Asset file not found: {}", fullPath.string());
            return AssetHandle{ UUID(0), 0 };
        }

        // 1. Try to load existing Metadata
        AssetMetadata metadata;
        metadata.Type = type;
        metadata.FilePath = path; // Relative path for the struct

        if (AssetSerializer::HasMetadata(fullPath))
        {
            AssetMetadata loadedMeta;
            if (AssetSerializer::TryLoadMetadata(fullPath, loadedMeta))
            {
                metadata.ID = loadedMeta.ID;
            }
        }

        // 2. If no ID (no metadata), generate new and save it
        if (!metadata.ID.IsValid())
        {
            metadata.ID = UUID(); // New Random UUID
            
            // Need absolute path for writing sidecar
            AssetMetadata writeMeta = metadata;
            writeMeta.FilePath = fullPath; 
            
            AssetSerializer::WriteMetadata(writeMeta);
            OPAL_INFO("Core/Assets", "Generated new metadata for '{}' (ID: {})", path.string(), (uint64_t)metadata.ID);
        }

        // 3. Check Cache
        Ref<IAsset> cachedAsset = m_AssetCache.Get(metadata.ID);
        if (cachedAsset) return AssetHandle{ metadata.ID, cachedAsset->GetMagic() };

        // 4. Load from disk
        Ref<IAsset> newAsset = LoadAssetInternal(type, path, metadata.ID);
        if (newAsset) return AssetHandle{ metadata.ID, newAsset->GetMagic() };

        return AssetHandle{ UUID(0), 0 };
    }

    Ref<IAsset> AssetManager::LoadAssetInternal(AssetType type, const std::filesystem::path& path, UUID id)
    {
        const char* typeString = Utils::AssetTypeToString(type);
        if (m_Serializers.find(type) == m_Serializers.end())
        {
            OPAL_ERROR("Core/Assets", "No serializer registered for AssetType='{}'", typeString);
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

        AssetSerializer& serializer = *m_Serializers[type];
        Ref<IAsset> asset = serializer.Load(stream, metadata, &m_LoadingArena);
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
