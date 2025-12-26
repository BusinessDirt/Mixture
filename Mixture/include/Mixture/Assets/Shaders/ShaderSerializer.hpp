#pragma once

/**
 * @file ShaderSerializer.hpp
 * @brief Asset serializer for Shader assets.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/AssetSerializer.hpp"

namespace Mixture
{
    /**
     * @brief Loads ShaderAssets from files.
     */
    class ShaderSerializer : public AssetSerializer
    {
    public:
        virtual ~ShaderSerializer() = default;

        Ref<IAsset> Load(FileStreamReader& stream, const AssetMetadata& metadata, ArenaAllocator* allocator = nullptr) override;
    };
}
