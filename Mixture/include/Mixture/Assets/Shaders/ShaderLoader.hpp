#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/IAssetLoader.hpp"

namespace Mixture
{
    class ShaderLoader : public IAssetLoader
    {
    public:
        virtual ~ShaderLoader() = default;

        Ref<IAsset> LoadSync(FileStreamReader& stream, const AssetMetadata& metadata) override;
    };
}
