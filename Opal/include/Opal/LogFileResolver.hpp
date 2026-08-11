#pragma once

#include <filesystem>
#include <memory>

namespace Opal
{
    /** Resolves a file-sink filename to a writable path. */
    class ILogFileResolver
    {
    public:
        virtual ~ILogFileResolver() = default;
        virtual std::filesystem::path Resolve(const std::filesystem::path& filename) const = 0;

        /** Creates the platform's default log file resolver. */
        static std::shared_ptr<ILogFileResolver> Create();
    };
}
