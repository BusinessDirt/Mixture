#pragma once

/**
 * @file AssetManager.hpp
 * @brief Singleton class for managing asset loading and retrieval.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/IAsset.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"
#include "Mixture/Core/Memory/LRUCache.hpp"

#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Util/FileSystemWatcher.hpp"

#include <array>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

namespace Mixture
{
    /**
     * @brief Manages the lifecycle and loading of assets.
     * 
     * Handles loading assets from disk, caching them, and providing access via handles.
     * Uses a dedicated I/O thread to prevent blocking the main TaskSystem.
     */
    class AssetManager
    {
    public:
        /**
         * @brief Gets the singleton instance of the AssetManager.
         * 
         * @return AssetManager& Reference to the instance.
         */
        static AssetManager& Get() { static AssetManager instance; return instance; }

        AssetManager() 
            : m_AssetCache(512 * 1024 * 1024) // 512MB Asset Cache by default
        {} 

        /**
         * @brief Initializes the AssetManager, registers loaders, and starts the I/O thread.
         */
        void Init();

        /**
         * @brief Shuts down the I/O thread and cleans up resources.
         */
        void Shutdown();

        /**
         * @brief Sets the root directory for asset lookups.
         * 
         * @param rootPath The root directory path.
         */
        void SetAssetRoot(const std::filesystem::path& rootPath);

        /**
         * @brief Sets the memory limit for the asset cache.
         * 
         * @param sizeInBytes Maximum memory in bytes.
         */
        void SetCacheSize(size_t sizeInBytes);

        /**
         * @brief Sets the current graphics API for API-specific asset loading (e.g. Shaders).
         * 
         * @param api The graphics API.
         */
        void SetGraphicsAPI(RHI::GraphicsAPI api) { m_GraphicsAPI = api; }

        /**
         * @brief Gets the current graphics API.
         * 
         * @return RHI::GraphicsAPI The graphics API.
         */
        RHI::GraphicsAPI GetGraphicsAPI() const { return m_GraphicsAPI; }

        /**
         * @brief Retrieves a handle to an asset, queuing it for load if necessary.
         * 
         * @param type The type of asset to load.
         * @param path The path to the asset file, relative to the asset root.
         * @return AssetHandle The handle to the asset.
         */
        AssetHandle GetAsset(AssetType type, const std::filesystem::path& path);

        /**
         * @brief Retrieves the raw asset pointer from a handle.
         * 
         * @tparam T The specific asset type (e.g., Texture, ShaderAsset).
         * @param handle The asset handle.
         * @return Ref<T> Pointer to the asset, or nullptr if invalid.
         */
        template<typename T>
        Ref<T> GetResource(AssetHandle handle)
        {
            if (!handle.ID.IsValid()) return nullptr;

            Ref<IAsset> asset = GetAssetFromCache(handle.ID);
            if (asset)
            {
                // Validate Magic Number
                if (asset->GetMagic() != handle.Magic)
                {
                    // Handle is stale (refers to an old instance of the asset)
                    return nullptr; 
                }
                return std::static_pointer_cast<T>(asset);
            }

            return nullptr;
        }

        /**
         * @brief Checks if an asset with the given ID is currently loaded in memory.
         * 
         * @param id The asset ID.
         * @return true If the asset is loaded.
         */
        bool IsAssetLoaded(UUID id);

    private:
        void LoadAssetInternal(AssetType type, const std::filesystem::path& path, UUID id, uint32_t magic);
        Ref<IAsset> GetAssetFromCache(UUID id);

    private:
        struct LoadRequest
        {
            AssetType Type;
            std::filesystem::path Path;
            UUID ID;
            uint32_t Magic;
        };

        std::filesystem::path m_RootDirectory;
        RHI::GraphicsAPI m_GraphicsAPI;

        // Cache State
        std::mutex m_CacheMutex;
        LRUCache<UUID, Ref<IAsset>> m_AssetCache;
        std::unordered_map<UUID, uint32_t> m_LoadingAssets; // Protected by m_CacheMutex
        std::unordered_map<AssetType, Scope<AssetSerializer>> m_Serializers;

        // I/O Thread State
        std::thread m_WorkerThread;
        std::mutex m_QueueMutex;
        std::condition_variable m_QueueCV;
        std::queue<LoadRequest> m_LoadQueue;
        std::atomic<bool> m_Running = false;

        Scope<FileSystemWatcher> m_FileWatcher;
    };
}
