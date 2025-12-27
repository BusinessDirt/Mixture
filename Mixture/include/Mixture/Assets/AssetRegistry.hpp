#pragma once

/**
 * @file AssetRegistry.hpp
 * @brief Registry for mapping Asset UUIDs to their metadata and file paths.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/IAsset.hpp"

#include <unordered_map>
#include <filesystem>

namespace Mixture
{
    /**
     * @brief Singleton registry maintaining a mapping of all available assets.
     */
    class AssetRegistry
    {
    public:
        /**
         * @brief Gets the singleton instance of the AssetRegistry.
         * 
         * @return AssetRegistry& Reference to the instance.
         */
        static AssetRegistry& Get() { static AssetRegistry instance; return instance; }

        AssetRegistry() = default;

        /**
         * @brief Registers an asset into the registry.
         * 
         * @param metadata The metadata of the asset to register.
         */
        void RegisterAsset(const AssetMetadata& metadata);

        /**
         * @brief Unregisters an asset from the registry.
         * 
         * @param id The UUID of the asset to remove.
         */
        void UnregisterAsset(UUID id);

        /**
         * @brief Checks if an asset exists in the registry.
         * 
         * @param id The UUID to check.
         * @return true If the asset is registered.
         */
        bool Contains(UUID id) const;

        /**
         * @brief Retrieves metadata for a given asset ID.
         * 
         * @param id The UUID of the asset.
         * @return const AssetMetadata& The metadata. Returns invalid metadata if not found.
         */
        const AssetMetadata& GetMetadata(UUID id) const;

        /**
         * @brief Retrieves the file path for a given asset ID.
         * 
         * @param id The UUID of the asset.
         * @return const std::filesystem::path& The file path. Returns empty path if not found.
         */
        const std::filesystem::path& GetPath(UUID id) const;

        /**
         * @brief Access the entire registry map.
         * 
         * @return const std::unordered_map<UUID, AssetMetadata>& The registry.
         */
        const std::unordered_map<UUID, AssetMetadata>& GetAssets() const { return m_Assets; }

        /**
         * @brief Clears the registry.
         */
        void Clear();

    private:
        std::unordered_map<UUID, AssetMetadata> m_Assets;
    };
}
