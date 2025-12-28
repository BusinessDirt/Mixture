#pragma once

/**
 * @file TextureSerializer.hpp
 * @brief Asset serializer for Texture assets.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"

namespace Mixture
{
    /**
     * @brief Loads TextureAssets from files using stb_image.
     */
    class TextureSerializer : public AssetSerializer
    {
    public:
        virtual ~TextureSerializer() = default;

        /**
         * @brief Loads a texture asset from raw data (e.g. PNG/JPG) using stb_image.
         * 
         * @param data The raw file data.
         * @param metadata The asset metadata.
         * @return Ref<IAsset> The loaded texture asset.
         */
        Ref<IAsset> Load(const Vector<char>& data, const AssetMetadata& metadata) override;
    };
}
