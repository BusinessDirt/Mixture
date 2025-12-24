#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Util/FileStreamReader.hpp"
#include "Mixture/Assets/IAsset.hpp"

namespace Mixture
{
    class IAssetLoader
    {
    public:
        virtual ~IAssetLoader() = default;

        virtual Ref<IAsset> LoadSync(FileStreamReader& stream, const AssetMetadata& metadata) = 0;
    };
}
