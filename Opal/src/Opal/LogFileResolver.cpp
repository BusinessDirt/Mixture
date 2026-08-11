#include "Opal/LogFileResolver.hpp"

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
    }

    std::shared_ptr<ILogFileResolver> ILogFileResolver::Create()
    {
        return std::make_shared<DirectoryLogFileResolver>();
    }
}
