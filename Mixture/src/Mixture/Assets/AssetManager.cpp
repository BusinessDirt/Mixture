#include "mxpch.hpp"
#include "Mixture/Assets/AssetManager.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"
#include "Mixture/Assets/AssetRegistry.hpp"

#include "Mixture/Assets/Shaders/ShaderSerializer.hpp"
#include "Mixture/Assets/Textures/TextureSerializer.hpp"

#include <fstream>
#include <limits>

namespace Mixture
{
    namespace
    {
        bool IsPathWithin(const std::filesystem::path& root, const std::filesystem::path& candidate)
        {
            auto rootIt = root.begin();
            auto candidateIt = candidate.begin();
            for (; rootIt != root.end(); ++rootIt, ++candidateIt)
            {
                if (candidateIt == candidate.end() || *candidateIt != *rootIt) return false;
            }
            return true;
        }
    }

    void AssetManager::Init()
    {
        std::lock_guard<std::mutex> lifecycleLock(m_LifecycleMutex);
        if (m_Initialized)
        {
            OPAL_LOG_DEBUG("AssetManager", "Init ignored because the service is already running.");
            return;
        }

        m_Serializers[AssetType::Shader] = CreateScope<ShaderSerializer>();
        m_Serializers[AssetType::Texture] = CreateScope<TextureSerializer>();

        m_AssetCache.SetEvictionCallback([](const UUID& id, const Ref<IAsset>& asset)
        {
            OPAL_LOG_DEBUG("AssetManager", "Evicting Asset '{}' (ID: {}) Size: {} bytes",
                asset ? asset->GetName() : "Unknown",
                (uint64_t)id,
                asset ? asset->GetMemoryUsage() : 0);
        });

        // Start I/O Thread
        m_Running = true;
        try
        {
            m_WorkerThread = std::thread([this]()
            {
                Opal::LogRegistry::SetThreadName("I/O Thread");
                OPAL_INFO("AssetManager", "I/O Thread Started.");

                while (true)
                {
                    LoadRequest request;
                    {
                        std::unique_lock<std::mutex> lock(m_QueueMutex);

                        m_QueueCV.wait(lock, [this] {
                            return !m_LoadQueue.empty() || !m_Running;
                        });

                        if (!m_Running)
                            return;

                        if (m_LoadQueue.empty())
                            continue;

                        request = m_LoadQueue.front();
                        m_LoadQueue.pop();
                        m_ActiveLoadID = request.ID;
                        m_ActiveLoadCancellable = true;
                    }

                    try
                    {
                        this->LoadAssetInternal(request.Type, request.Path, request.ID, request.Magic);
                    }
                    catch (const std::exception& e)
                    {
                        OPAL_ERROR("AssetManager", "Asset Load Dispatch Exception: {}", e.what());

                        std::lock_guard<std::mutex> lock(m_CacheMutex);
                        m_LoadingAssets.erase(request.ID);
                    }

                    {
                        std::lock_guard<std::mutex> lock(m_QueueMutex);
                        m_ActiveLoadID = UUID::Invalid();
                        m_ActiveLoadCancellable = false;
                        m_CancelledLoads.erase(request.ID);
                        if (m_LoadQueue.empty()) m_IdleCV.notify_all();
                    }
                }
            });
        }
        catch (...)
        {
            m_Running = false;
            throw;
        }

        m_Initialized = true;
        m_MetadataFileAccessCount = 0;
    }

    void AssetManager::Shutdown()
    {
        std::lock_guard<std::mutex> lifecycleLock(m_LifecycleMutex);
        if (!m_Initialized) return;

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

            OPAL_INFO("AssetManager", "I/O Thread Shutdown.");
        }

        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            std::queue<LoadRequest> emptyQueue;
            m_LoadQueue.swap(emptyQueue);
            m_CancelledLoads.clear();
            m_ActiveLoadID = UUID::Invalid();
            m_ActiveLoadCancellable = false;
            m_IdleCV.notify_all();
        }
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.clear();
            m_AssetCache.Clear();
        }
        {
            std::lock_guard<std::mutex> lock(m_CallbackMutex);
            m_ReloadCallbacks.clear();
            m_NextReloadCallbackHandle = 1;
        }

        AssetRegistry::Get().Clear();
        m_RootDirectory.clear();
        m_GraphicsAPI = RHI::GraphicsAPI::None;
        m_Initialized = false;
    }

    void AssetManager::WaitForIdle()
    {
        std::unique_lock<std::mutex> lock(m_QueueMutex);
        m_IdleCV.wait(lock, [this]
        {
            return m_LoadQueue.empty() && !m_ActiveLoadID.IsValid();
        });
    }

    bool AssetManager::CancelLoad(UUID id)
    {
        if (!id.IsValid()) return false;

        bool found = false;
        bool removedFromQueue = false;
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            found = m_ActiveLoadID == id && m_ActiveLoadCancellable;
            if (found) m_CancelledLoads.insert(id);

            std::queue<LoadRequest> retained;
            while (!m_LoadQueue.empty())
            {
                LoadRequest request = std::move(m_LoadQueue.front());
                m_LoadQueue.pop();
                if (request.ID == id)
                {
                    found = true;
                    removedFromQueue = true;
                }
                else
                {
                    retained.push(std::move(request));
                }
            }
            m_LoadQueue.swap(retained);

            if (m_LoadQueue.empty() && !m_ActiveLoadID.IsValid())
                m_IdleCV.notify_all();
        }

        if (removedFromQueue)
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(id);
        }
        return found;
    }

    void AssetManager::SetAssetRoot(const std::filesystem::path& rootPath)
    {
        std::error_code error;
        m_RootDirectory = std::filesystem::weakly_canonical(std::filesystem::absolute(rootPath), error);
        if (error)
        {
            OPAL_ERROR("AssetManager", "Failed to resolve asset directory '{}': {}", rootPath.string(), error.message());
            m_RootDirectory.clear();
            return;
        }
        OPAL_INFO("AssetManager", "Asset Directory set to: {}", m_RootDirectory.string());

        if (!std::filesystem::exists(m_RootDirectory))
        {
            OPAL_WARN("AssetManager", "Asset Directory does not exist!");
        }

        // Initialize File Watcher
        if (std::filesystem::exists(m_RootDirectory))
        {
            if (m_FileWatcher) m_FileWatcher->Stop();

            m_FileWatcher = CreateScope<FileSystemWatcher>(m_RootDirectory, [this](const std::filesystem::path& path, FileAction action)
            {
                this->OnAssetChange(path, action);
            });
            m_FileWatcher->Start();
        }
    }

    void AssetManager::SetCacheSize(size_t sizeInBytes)
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        m_AssetCache.SetMaxMemory(sizeInBytes);
    }

    AssetManager::ReloadCallbackHandle AssetManager::AddReloadCallback(AssetReloadCallback callback)
    {
        std::lock_guard<std::mutex> lock(m_CallbackMutex);
        const ReloadCallbackHandle handle = m_NextReloadCallbackHandle++;
        m_ReloadCallbacks.emplace(handle, std::move(callback));
        return handle;
    }

    bool AssetManager::RemoveReloadCallback(ReloadCallbackHandle handle)
    {
        std::lock_guard<std::mutex> lock(m_CallbackMutex);
        return m_ReloadCallbacks.erase(handle) != 0;
    }

    bool AssetManager::EnqueueLoad(LoadRequest request)
    {
        if (!m_Running) return false;

        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            if (!m_Running) return false;
            m_CancelledLoads.erase(request.ID);
            m_LoadQueue.push(std::move(request));
        }
        m_QueueCV.notify_one();
        return true;
    }

    bool AssetManager::IsLoadCancelled(UUID id)
    {
        if (!m_Running) return true;

        std::lock_guard<std::mutex> lock(m_QueueMutex);
        return m_CancelledLoads.contains(id);
    }

    std::optional<std::filesystem::path> AssetManager::ResolveFullPath(
        AssetType type, const std::filesystem::path& relativePath) const
    {
        if (m_RootDirectory.empty() || relativePath.empty() || relativePath.is_absolute()
            || type <= AssetType::None || type >= AssetType::Count)
            return std::nullopt;

        std::error_code error;
        const std::filesystem::path typeRoot = std::filesystem::weakly_canonical(
            m_RootDirectory / Utils::AssetTypeToString(type), error);
        if (error) return std::nullopt;

        const std::filesystem::path candidate = std::filesystem::weakly_canonical(typeRoot / relativePath, error);
        if (error || !IsPathWithin(typeRoot, candidate)) return std::nullopt;
        return candidate;
    }

    void AssetManager::OnAssetChange(const std::filesystem::path& path, FileAction action)
    {
        std::filesystem::path assetPath = path;
        const bool metadataChanged = assetPath.extension() == ".meta";
        if (metadataChanged) assetPath.replace_extension();

        AssetMetadata metadata;
        for (uint8_t value = static_cast<uint8_t>(AssetType::None) + 1;
             value < static_cast<uint8_t>(AssetType::Count); ++value)
        {
            const AssetType type = static_cast<AssetType>(value);
            const std::filesystem::path typeRoot = m_RootDirectory / Utils::AssetTypeToString(type);
            const std::filesystem::path relative = assetPath.lexically_relative(typeRoot);
            if (!relative.empty() && *relative.begin() != "..")
            {
                metadata = AssetRegistry::Get().FindByPath(type, relative);
                if (metadata.ID.IsValid()) break;
            }
        }

        if (!metadata.ID.IsValid()) return;
        if (metadataChanged || action == FileAction::Deleted)
        {
            AssetRegistry::Get().UnregisterAsset(metadata.ID);
            if (metadataChanged || action != FileAction::Modified) return;
        }
        if (action != FileAction::Modified) return;

        // If found, check if it's currently loaded
        if (IsAssetLoaded(metadata.ID))
        {
            Ref<IAsset> currentAsset = GetAssetFromCache(metadata.ID);
            if (currentAsset)
            {
                OPAL_INFO("AssetManager", "Reactive Reloading asset: {}", path.string());
                // Reuse existing magic to keep handles valid
                {
                    std::lock_guard<std::mutex> lock(m_CacheMutex);
                    if (m_LoadingAssets.contains(metadata.ID)) return;
                    m_LoadingAssets[metadata.ID] = currentAsset->GetMagic();
                }

                if (!EnqueueLoad({ metadata.Type, metadata.FilePath, metadata.ID, currentAsset->GetMagic() }))
                {
                    std::lock_guard<std::mutex> lock(m_CacheMutex);
                    m_LoadingAssets.erase(metadata.ID);
                }
            }
        }
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

        AssetMetadata metadata = AssetRegistry::Get().FindByPath(type, resolvedPath);

        const auto fullPath = ResolveFullPath(type, resolvedPath);
        if (!fullPath)
        {
            OPAL_ERROR("AssetManager", "Rejected asset path outside its type root: {}", path.string());
            return AssetHandle{ UUID::Invalid(), 0 };
        }

        // Note: Checking file existence on disk is slow and blocking.
        // Optimally, we would trust the registry or metadata, but for now we keep it.
        // However, we should avoid doing it inside a lock if possible.

        // Try to load existing Metadata (Blocking I/O but small)
        // We do this BEFORE locking because it involves I/O.
        if (!metadata.ID.IsValid())
        {
            ++m_MetadataFileAccessCount;
            metadata.Type = type;
            metadata.FilePath = resolvedPath;

            if (AssetSerializer::HasMetadata(*fullPath))
            {
                AssetMetadata loadedMeta;
                if (!AssetSerializer::TryLoadMetadata(*fullPath, loadedMeta) || loadedMeta.Type != type)
                {
                    OPAL_ERROR("AssetManager", "Invalid or mismatched asset metadata: {}.meta", fullPath->string());
                    return AssetHandle{ UUID::Invalid(), 0 };
                }
                metadata.ID = loadedMeta.ID;
            }

            // If no ID (no metadata), generate new and save it
            if (!metadata.ID.IsValid())
            {
                if (!std::filesystem::exists(*fullPath))
                {
                    OPAL_ERROR("AssetManager", "Asset file not found: {}", fullPath->string());
                    return AssetHandle{ UUID(0), 0 };
                }

                metadata.ID = UUID();

                AssetMetadata writeMeta = metadata;
                writeMeta.FilePath = *fullPath;

                AssetSerializer::WriteMetadata(writeMeta);
                OPAL_INFO("AssetManager", "Generated new metadata for '{}' (ID: {})", resolvedPath.string(), (uint64_t)metadata.ID);
            }

            if (!AssetRegistry::Get().RegisterAsset(metadata))
                return AssetHandle{ UUID::Invalid(), 0 };
        }

        // Check Cache & Loading Map
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);

            // Check Cache
            Ref<IAsset> cachedAsset = m_AssetCache.Get(metadata.ID);
            if (cachedAsset) return AssetHandle{ metadata.ID, cachedAsset->GetMagic() };

            // Check if already loading
            auto it = m_LoadingAssets.find(metadata.ID);
            if (it != m_LoadingAssets.end()) return AssetHandle{ metadata.ID, 0 };
        }

        // Submit Load Task
        static std::atomic<uint32_t> s_AssetMagicCounter(1);
        uint32_t newMagic = s_AssetMagicCounter++;

        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            // Double check cache/loading just in case
            if (m_AssetCache.Contains(metadata.ID))
                 return AssetHandle{ metadata.ID, m_AssetCache.Get(metadata.ID)->GetMagic() };

            m_LoadingAssets[metadata.ID] = newMagic;
        }

        if (!EnqueueLoad({ type, resolvedPath, metadata.ID, newMagic }))
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(metadata.ID);
            OPAL_ERROR("AssetManager", "Cannot load asset while the I/O executor is stopped: {}", fullPath->string());
            return AssetHandle{ UUID::Invalid(), 0 };
        }

        // Return 0 magic to indicate loading
        return AssetHandle{ metadata.ID, 0 };
    }

    void AssetManager::LoadAssetInternal(AssetType type, const std::filesystem::path& path, UUID id, uint32_t magic)
    {
        const char* typeString = Utils::AssetTypeToString(type);
        auto serializerIt = m_Serializers.find(type);
        if (serializerIt == m_Serializers.end())
        {
            OPAL_ERROR("AssetManager", "No serializer registered for AssetType='{}'", typeString);
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(id);
            return;
        }

        const auto fullPath = ResolveFullPath(type, path);
        if (!fullPath)
        {
            OPAL_ERROR("AssetManager", "Rejected asset load outside its type root: {}", path.string());
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(id);
            return;
        }

        std::ifstream stream(*fullPath, std::ios::binary | std::ios::ate);
        if (!stream)
        {
            OPAL_ERROR("AssetManager", "Failed to open file {}", fullPath->string());
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(id);
            return;
        }

        const std::streampos end = stream.tellg();
        if (end <= 0)
        {
            OPAL_ERROR("AssetManager", "Read empty data or failed from file: {}", path.string());
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(id);
            return;
        }

        const auto endOffset = static_cast<std::streamoff>(end);
        const uintmax_t fileSize = static_cast<uintmax_t>(endOffset);
        if (fileSize > std::numeric_limits<size_t>::max()
            || fileSize > static_cast<uintmax_t>(std::numeric_limits<std::streamsize>::max()))
        {
            OPAL_ERROR("AssetManager", "Asset file is too large to read safely: {}", fullPath->string());
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(id);
            return;
        }

        Vector<char> data(static_cast<size_t>(endOffset));
        stream.seekg(0, std::ios::beg);

        constexpr size_t ReadChunkSize = 1024 * 1024;
        size_t offset = 0;
        while (offset < data.size())
        {
            if (IsLoadCancelled(id))
            {
                std::lock_guard<std::mutex> lock(m_CacheMutex);
                m_LoadingAssets.erase(id);
                return;
            }

            const size_t bytesToRead = std::min(ReadChunkSize, data.size() - offset);
            stream.read(data.data() + offset, static_cast<std::streamsize>(bytesToRead));
            if (stream.gcount() != static_cast<std::streamsize>(bytesToRead))
            {
                OPAL_ERROR("AssetManager", "Failed while reading file: {}", fullPath->string());
                std::lock_guard<std::mutex> lock(m_CacheMutex);
                m_LoadingAssets.erase(id);
                return;
            }
            offset += bytesToRead;
        }

        if (IsLoadCancelled(id))
        {
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
            asset = serializerIt->second->Load(data, metadata);
        }
        catch (const std::exception& e)
        {
            OPAL_ERROR("AssetManager", "Exception deserializing {}: {}", path.string(), e.what());
        }

        bool cancelled = false;
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            cancelled = !m_Running || m_CancelledLoads.contains(id);
            if (!cancelled) m_ActiveLoadCancellable = false;
        }

        bool notifyReload = false;
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_LoadingAssets.erase(id);

            if (asset && !cancelled)
            {
                asset->SetMagic(magic);
                m_AssetCache.Put(id, asset, asset->GetMemoryUsage());
                OPAL_LOG_DEBUG("AssetManager", "Loaded {} from '{}' (ID: {})", typeString, path.string(), (uint64_t)id);
                notifyReload = true;
            }
            else if (!cancelled)
            {
                OPAL_LOG_DEBUG("AssetManager", "Failed to reload asset: invalid data.");
            }
        }

        if (notifyReload && m_Running)
        {
            Vector<AssetReloadCallback> callbacks;
            {
                std::lock_guard<std::mutex> lock(m_CallbackMutex);
                callbacks.reserve(m_ReloadCallbacks.size());
                for (const auto& [handle, callback] : m_ReloadCallbacks)
                {
                    callbacks.push_back(callback);
                }
            }

            for (const auto& callback : callbacks)
            {
                callback(type, id);
            }
        }
    }
}
