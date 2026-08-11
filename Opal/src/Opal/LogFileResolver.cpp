#include "Opal/LogFileResolver.hpp"

#include "Opal/PlatformDetection.hpp"

namespace Opal
{
    namespace
    {
        class DirectoryLogFileResolver final : public ILogFileResolver
        {
        public:
            std::filesystem::path Resolve(const std::filesystem::path& filename) const override
            {
                const std::filesystem::path directory = "logs";
                std::filesystem::create_directories(directory);
                return directory / filename;
            }
        };

        class DarwinHomeDirectoryLogFileResolver final : public ILogFileResolver
        {
        public:
            std::filesystem::path Resolve(const std::filesystem::path& filename) const override
            {
                const char* homeDir = std::getenv("HOME");
                if (!homeDir) throw std::invalid_argument("Home directory not found");

                std::filesystem::path logDir = std::filesystem::path(homeDir) / "Library" / "Logs" / "MixtureEngine";
                std::filesystem::create_directories(logDir);

                return (logDir / filename).string();
            }
        };
    }

    std::shared_ptr<ILogFileResolver> ILogFileResolver::Create()
    {
#if defined(OPAL_PLATFORM_DARWIN)
        return std::make_shared<DarwinHomeDirectoryLogFileResolver>();
#else
        return std::make_shared<DirectoryLogFileResolver>();
#endif
    }
}
