#pragma once

/**
 * @file ShaderLoader.hpp
 * @brief Asset loader for Shader assets.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/IAssetLoader.hpp"

namespace Mixture
{
    /**
     * @brief Loads ShaderAssets from files.
     */
    class ShaderLoader : public IAssetLoader
    {
    public:
        virtual ~ShaderLoader() = default;

        Ref<IAsset> LoadSync(FileStreamReader& stream, const AssetMetadata& metadata, ArenaAllocator* allocator = nullptr) override;
    };
}
