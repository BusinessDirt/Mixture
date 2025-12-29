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

        /**
         * @brief Loads a shader asset from binary data (SPIR-V).
         * 
         * @param data The raw file data.
         * @param metadata The asset metadata.
         * @return Ref<IAsset> The loaded shader asset.
         */
        Ref<IAsset> Load(const Vector<char>& data, const AssetMetadata& metadata) override;
    };
}
