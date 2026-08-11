#pragma once

#include "Mixture/Assets/IAsset.hpp"
#include "Mixture/Core/Base.hpp"

#include <filesystem>
#include <optional>

namespace Mixture
{
    /** Resolves typed logical asset paths to paths readable by AssetManager. */
    class IAssetFileResolver
    {
    public:
        virtual ~IAssetFileResolver() = default;

        virtual std::optional<std::filesystem::path> Resolve(
            AssetType type, const std::filesystem::path& path) const = 0;

        /** Creates the platform's default asset file resolver. */
        static Ref<IAssetFileResolver> Create(const std::filesystem::path& root);
    };
}
