#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/IAsset.hpp"
#include "Mixture/Util/FileStreamReader.hpp"
#include "Mixture/Core/Memory/ArenaAllocator.hpp"

namespace Mixture
{
    /**
     * @brief Base interface for asset serialization (Loading & Saving).
     * Also contains static utilities for handling sidecar .meta files.
     */
    class AssetSerializer
    {
    public:
        virtual ~AssetSerializer() = default;

        /**
         * @brief Loads an asset synchronously from a file stream.
         * 
         * @param stream The file stream to read from.
         * @param metadata Metadata about the asset being loaded.
         * @param allocator Optional arena allocator for temporary scratch memory.
         * @return Ref<IAsset> The loaded asset.
         */
        virtual Ref<IAsset> Load(FileStreamReader& stream, const AssetMetadata& metadata, ArenaAllocator* allocator = nullptr) = 0;

        /**
         * @brief Saves an asset to disk. (Optional/Future Implementation)
         */
        virtual void Save(const std::filesystem::path& path, const Ref<IAsset>& asset) 
        { 
            OPAL_WARN("Core/Assets", "Save not implemented for this asset type."); 
        }

        // --- Static Metadata Helpers ---

        /**
         * @brief Tries to load metadata from a sidecar file (.meta).
         * @param assetPath Path to the source asset file.
         * @param outMetadata Output metadata struct.
         * @return true If metadata file existed and was loaded successfully.
         */
        static bool TryLoadMetadata(const std::filesystem::path& assetPath, AssetMetadata& outMetadata);

        /**
         * @brief Writes metadata to a sidecar file (.meta).
         * @param metadata The metadata to write.
         */
        static void WriteMetadata(const AssetMetadata& metadata);

        /**
         * @brief Checks if a metadata file exists for the given asset.
         */
        static bool HasMetadata(const std::filesystem::path& assetPath);

    private:
        static std::filesystem::path GetMetadataPath(const std::filesystem::path& assetPath);
    };
}
