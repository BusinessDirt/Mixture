#pragma once

/**
 * @file IAssetLoader.hpp
 * @brief Interface for asset loaders.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/Memory/ArenaAllocator.hpp"

#include "Mixture/Util/FileStreamReader.hpp"
#include "Mixture/Assets/IAsset.hpp"

namespace Mixture
{
    /**
     * @brief Interface for classes that load specific asset types from a stream.
     */
    class IAssetLoader
    {
    public:
        virtual ~IAssetLoader() = default;

        /**
         * @brief Loads an asset synchronously from a file stream.
         * 
         * @param stream The file stream to read from.
         * @param metadata Metadata about the asset being loaded.
         * @param allocator Optional arena allocator for temporary scratch memory.
         * @return Ref<IAsset> The loaded asset.
         */
        virtual Ref<IAsset> LoadSync(FileStreamReader& stream, const AssetMetadata& metadata, ArenaAllocator* allocator = nullptr) = 0;
    };
}
