#include "mxpch.hpp"
#include "Mixture/Util/Util.hpp"

namespace Mixture::Util
{
    bool Contains(const std::string& str, const std::string& substr)
    {
        return str.find(substr) != std::string::npos;
    }

    std::string ReadFile(const std::filesystem::path& filepath)
    {
        std::string result;

        if (std::ifstream in(filepath, std::ios::in | std::ios::binary); in)
        {
            in.seekg(0, std::ios::end);

            if (const size_t size = in.tellg(); std::cmp_not_equal(size, -1))
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(result.data(), static_cast<std::streamsize>(size));
            }
            else OPAL_ERROR("Core", "Mixture::Util::ReadFile() - Could not read from file '{0}'", filepath.string().c_str());

        }
        else OPAL_ERROR("Core", "Mixture::Util::ReadFile() - Could not open file '{0}'", filepath.string().c_str());

        return result;
    }
}
