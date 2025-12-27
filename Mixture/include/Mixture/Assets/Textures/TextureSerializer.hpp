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

        Ref<IAsset> Load(FileStreamReader& stream, const AssetMetadata& metadata, ArenaAllocator* allocator = nullptr) override;
    };
}
