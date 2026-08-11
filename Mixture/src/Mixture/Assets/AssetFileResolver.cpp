#include "mxpch.hpp"
#include "Mixture/Assets/AssetFileResolver.hpp"

#if defined(OPAL_PLATFORM_DARWIN)
#include <mach-o/dyld.h>
#include <climits>
#endif

namespace Mixture
{
    namespace
    {
        bool IsPathWithin(const std::filesystem::path& root, const std::filesystem::path& candidate)
        {
            auto rootIt = root.begin();
            auto candidateIt = candidate.begin();
            for (; rootIt != root.end(); ++rootIt, ++candidateIt)
            {
                if (candidateIt == candidate.end() || *candidateIt != *rootIt) return false;
            }
            return true;
        }

        class FileSystemAssetFileResolver final : public IAssetFileResolver
        {
        public:
            explicit FileSystemAssetFileResolver(std::filesystem::path root)
                : m_Root(std::move(root))
            {}

            std::optional<std::filesystem::path> Resolve(AssetType type, const std::filesystem::path& path) const override
            {
                if (m_Root.empty() || path.empty() || path.is_absolute()
                    || type <= AssetType::None || type >= AssetType::Count)
                    return std::nullopt;

                std::error_code error;
                const std::filesystem::path typeRoot = std::filesystem::weakly_canonical(
                    m_Root / Utils::AssetTypeToString(type), error);
                if (error) return std::nullopt;

                const std::filesystem::path candidate = std::filesystem::weakly_canonical(typeRoot / path, error);
                if (error || !IsPathWithin(typeRoot, candidate)) return std::nullopt;
                return candidate;
            }

        private:
            std::filesystem::path m_Root;
        };

#if defined(OPAL_PLATFORM_DARWIN)

    class AppBundleAssetFileResolver final : public IAssetFileResolver
    {
    public:
        AppBundleAssetFileResolver() : m_Resolver(GetBundleAssetRoot()) {}

        std::optional<std::filesystem::path> Resolve(AssetType type, const std::filesystem::path& path) const override
        {
            return m_Resolver.Resolve(type, path);
        }

    private:
        FileSystemAssetFileResolver m_Resolver;

        static std::filesystem::path GetBundleAssetRoot()
        {
            char path[PATH_MAX];
            uint32_t size = sizeof(path);
            if (_NSGetExecutablePath(path, &size) == 0)
            {
                std::filesystem::path exePath = std::filesystem::weakly_canonical(path);
                // Steps back from App.app/Contents/MacOS/App to App.app/Contents/Resources/Assets
                return exePath.parent_path().parent_path() / "Resources" / "Assets";
            }

            // Fallback just in case
            return std::filesystem::path("Assets");
        }
    };
#endif // defined(OPAL_PLATFORM_DARWIN)
    }

    Ref<IAssetFileResolver> IAssetFileResolver::Create(const std::filesystem::path& root)
    {
#if defined(OPAL_PLATFORM_DARWIN)
        return CreateRef<AppBundleAssetFileResolver>();
#else
        return CreateRef<FileSystemAssetFileResolver>(root);
#endif
    }
}
