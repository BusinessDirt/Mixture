#pragma once

/**
 * @file AssetManager.hpp
 * @brief Singleton class for managing asset loading and retrieval.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/IAsset.hpp"
#include "Mixture/Assets/IAssetLoader.hpp"
#include "Mixture/Core/Memory/ArenaAllocator.hpp"

#include "Mixture/Render/RHI/IGraphicsContext.hpp"

#include <array>

namespace Mixture
{
    /**
     * @brief Manages the lifecycle and loading of assets.
     * 
     * Handles loading assets from disk, caching them, and providing access via handles.
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

        AssetManager() : m_LoadingArena(1024 * 1024) {} // 1MB Scratchpad

        /**
         * @brief Initializes the AssetManager and registers default loaders.
         */
        void Init();

        /**
         * @brief Sets the root directory for asset lookups.
         * 
         * @param rootPath The root directory path.
         */
        void SetAssetRoot(const std::filesystem::path& rootPath);

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
         * @brief Retrieves a handle to an asset, loading it if necessary.
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

            auto it = m_Assets.find(handle.ID);
            if (it != m_Assets.end())
            {
                // Validate Magic Number
                if (it->second->GetMagic() != handle.Magic)
                {
                    // Handle is stale (refers to an old instance of the asset)
                    return nullptr; 
                }
                return std::static_pointer_cast<T>(it->second);
            }

            return nullptr;
        }

    private:
        Ref<IAsset> LoadAssetInternal(AssetType type, const std::filesystem::path& path, UUID id);

    private:
        std::filesystem::path m_RootDirectory;
        RHI::GraphicsAPI m_GraphicsAPI;

        std::unordered_map<UUID, Ref<IAsset>> m_Assets;
        std::unordered_map<AssetType, Scope<IAssetLoader>> m_Loaders;

        ArenaAllocator m_LoadingArena;
    };
}
