#include "mxpch.hpp"
#include "Mixture/Assets/AssetFileResolver.hpp"

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

            std::optional<std::filesystem::path> Resolve(
                AssetType type, const std::filesystem::path& path) const override
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
    }

    Ref<IAssetFileResolver> IAssetFileResolver::Create(const std::filesystem::path& root)
    {
        return CreateRef<FileSystemAssetFileResolver>(root);
    }
}
