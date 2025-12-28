#include "mxpch.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"
#include "Mixture/Assets/AssetRegistry.hpp"

#include "Mixture/Assets/Shaders/ShaderSerializer.hpp"
#include "Mixture/Assets/Textures/TextureSerializer.hpp"

#include "Mixture/Util/AsyncFileReader.hpp"
#include <future>

namespace Mixture
{
    void AssetManager::Init()
    {
        m_Serializers[AssetType::Shader] = CreateScope<ShaderSerializer>();
        m_Serializers[AssetType::Texture] = CreateScope<TextureSerializer>();

        m_AssetCache.SetEvictionCallback([](const UUID& id, const Ref<IAsset>& asset)
        {
            OPAL_LOG_DEBUG("Core/Assets", "Evicting Asset '{}' (ID: {}) Size: {} bytes",
                asset ? asset->GetName() : "Unknown",
                (uint64_t)id,
                asset ? asset->GetMemoryUsage() : 0);
        });

        // Start I/O Thread
        m_Running = true;
        m_WorkerThread = std::thread([this]()
        {
            Opal::LogRegistry::SetThreadName("I/O Thread");
            OPAL_INFO("Core/Assets", "I/O Thread Started.");

            while (m_Running)
            {
                LoadRequest request;
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);

                    m_QueueCV.wait(lock, [this] {
                        return !m_LoadQueue.empty() || !m_Running;
                    });

                    if (!m_Running && m_LoadQueue.empty())
                        return;

                    if (m_LoadQueue.empty())
                        continue;

                    request = m_LoadQueue.front();
                    m_LoadQueue.pop();
                }

                try
                {
                    this->LoadAssetInternal(request.Type, request.Path, request.ID, request.Magic);
                }
                catch (const std::exception& e)
                {
                    OPAL_ERROR("Core/Assets", "Asset Load Dispatch Exception: {}", e.what());

                    std::lock_guard<std::mutex> lock(m_CacheMutex);
                    m_LoadingAssets.erase(request.ID);
                }
            }
        });
    }

    void AssetManager::Shutdown()
    {
        if (m_FileWatcher)
        {
            m_FileWatcher->Stop();
            m_FileWatcher.reset();
        }

        if (m_Running)
        {
            m_Running = false;
            m_QueueCV.notify_all();

            if (m_WorkerThread.joinable())
                m_WorkerThread.join();

            OPAL_INFO("Core/Assets", "I/O Thread Shutdown.");
        }
    }

    void AssetManager::SetAssetRoot(const std::filesystem::path& rootPath)
    {
        m_RootDirectory = std::filesystem::absolute(rootPath);
        OPAL_INFO("Core/Assets", "Asset Directory set to: {}", m_RootDirectory.string());

        if (!std::filesystem::exists(m_RootDirectory))
        {
            OPAL_WARN("Core/Assets", "Asset Directory does not exist!");
        }

        // Initialize File Watcher
        if (std::filesystem::exists(m_RootDirectory))
        {
            if (m_FileWatcher) m_FileWatcher->Stop();

            m_FileWatcher = CreateScope<FileSystemWatcher>(m_RootDirectory, [](const std::filesystem::path& path, FileAction action)
            {
                const char* actionStr = "Unknown";
                switch(action)
                {
                    case FileAction::Added: actionStr = "Added"; break;
                    case FileAction::Modified: actionStr = "Modified"; break;
                    case FileAction::Deleted: actionStr = "Deleted"; break;
                }
                OPAL_INFO("Core/Assets", "Asset File Changed: {} ({})", path.string(), actionStr);
                
                // TODO: Trigger hot-reload logic here
            });
            m_FileWatcher->Start();
        }
    }

    void AssetManager::SetCacheSize(size_t sizeInBytes)
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        m_AssetCache.SetMaxMemory(sizeInBytes);
    }

    Ref<IAsset> AssetManager::GetAssetFromCache(UUID id)
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        return m_AssetCache.Get(id);
    }

    bool AssetManager::IsAssetLoaded(UUID id)
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        return m_AssetCache.Contains(id);
    }

    AssetHandle AssetManager::GetAsset(AssetType type, const std::filesystem::path& path)
    {
        // Resolve path through redirectors in case the asset has moved
        std::filesystem::path resolvedPath = AssetRegistry::Get().ResolvePath(type, path);

        const char* typeString = Utils::AssetTypeToString(type);
        std::filesystem::path fullPath = m_RootDirectory / typeString / resolvedPath;

        // Note: Checking file existence on disk is slow and blocking.
        // Optimally, we would trust the registry or metadata, but for now we keep it.
        // However, we should avoid doing it inside a lock if possible.

        // 1. Try to load existing Metadata (Blocking I/O but small)
        // We do this BEFORE locking because it involves I/O.
        AssetMetadata metadata;
        metadata.Type = type;
        metadata.FilePath = resolvedPath;

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
            // If file doesn't exist, we can't really load it.
            if (!std::filesystem::exists(fullPath))
            {
                OPAL_ERROR("Core/Assets", "Asset file not found: {}", fullPath.string());
                return AssetHandle{ UUID(0), 0 };
            }

            metadata.ID = UUID(); // New Random UUID

            // Need absolute path for writing sidecar
            AssetMetadata writeMeta = metadata;
            writeMeta.FilePath = fullPath;

            AssetSerializer::WriteMetadata(writeMeta);
            OPAL_INFO("Core/Assets", "Generated new metadata for '{}' (ID: {})", resolvedPath.string(), (uint64_t)metadata.ID);
        }

        // 3. Check Cache & Loading Map
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);

            // Check Cache
            Ref<IAsset> cachedAsset = m_AssetCache.Get(metadata.ID);
            if (cachedAsset) return AssetHandle{ metadata.ID, cachedAsset->GetMagic() };

            // Check if already loading
            auto it = m_LoadingAssets.find(metadata.ID);
            if (it != m_LoadingAssets.end())
            {
                // Return handle with 0 magic (loading)
                return AssetHandle{ metadata.ID, 0 };
            }
        }

        // 4. Submit Load Task
        static std::atomic<uint32_t> s_AssetMagicCounter(1);
        uint32_t newMagic = s_AssetMagicCounter++;

        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            // Double check cache/loading just in case
            if (m_AssetCache.Contains(metadata.ID))
                 return AssetHandle{ metadata.ID, m_AssetCache.Get(metadata.ID)->GetMagic() };

            m_LoadingAssets[metadata.ID] = newMagic;
        }

        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_LoadQueue.push({ type, resolvedPath, metadata.ID, newMagic });
        }
        m_QueueCV.notify_one();

        // Return 0 magic to indicate loading
        return AssetHandle{ metadata.ID, 0 };
    }

    void AssetManager::LoadAssetInternal(AssetType type, const std::filesystem::path& path, UUID id, uint32_t magic)
    {
        const char* typeString = Utils::AssetTypeToString(type);
        if (m_Serializers.find(type) == m_Serializers.end())
        {
            OPAL_ERROR("Core/Assets", "No serializer registered for AssetType='{}'", typeString);
            {
                 std::lock_guard<std::mutex> lock(m_CacheMutex);
                 m_LoadingAssets.erase(id);
            }
            return;
        }

        std::filesystem::path fullPath = m_RootDirectory / typeString / path;
        
        // Use shared_ptr to keep the reader (and its internal file handle) alive until the callback executes
        auto reader = std::make_shared<AsyncFileReader>(fullPath);

        if (!reader->IsOpen())
        {
            OPAL_ERROR("Core/Assets", "Failed to open file {}", fullPath.string());
            {
                 std::lock_guard<std::mutex> lock(m_CacheMutex);
                 m_LoadingAssets.erase(id);
            }
            return;
        }

        // Initiate Async Read
        // The callback captures 'this' and metadata, effectively processing deserialization on the I/O completion thread
        reader->ReadBuffer([this, reader, type, path, id, magic, typeString](Vector<char> data)
        {
            // This callback is executed when the read is done.
            // reader is captured to ensure it stays alive until here.
            
            if (data.empty())
            {
                OPAL_ERROR("Core/Assets", "Read empty data or failed from file: {}", path.string());
                std::lock_guard<std::mutex> lock(m_CacheMutex);
                m_LoadingAssets.erase(id);
                return;
            }

            AssetMetadata metadata;
            metadata.ID = id;
            metadata.Type = type;
            metadata.FilePath = path;

            Ref<IAsset> asset = nullptr;

            try
            {
                // We assume serializers are thread-safe or we are the only one accessing this specific serializer instance via read-only map
                // But Serializers themselves are stateless usually, except for base configuration.
                AssetSerializer& serializer = *m_Serializers[type];
                asset = serializer.Load(data, metadata);
            }
            catch (const std::exception& e)
            {
                OPAL_ERROR("Core/Assets", "Exception deserializing {}: {}", path.string(), e.what());
            }

            // Update Cache
            {
                std::lock_guard<std::mutex> lock(m_CacheMutex);
                m_LoadingAssets.erase(id);

                if (asset)
                {
                    asset->SetMagic(magic);
                    m_AssetCache.Put(id, asset, asset->GetMemoryUsage());
                    OPAL_LOG_DEBUG("Core/Assets", "Async Loaded {} from '{}' (ID: {})", typeString, path.string(), (uint64_t)id);
                }
            }
        });
    }
}
