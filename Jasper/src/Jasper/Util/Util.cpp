#include "Jasper/Util/Util.hpp"

#include "Opal/Base.hpp"

namespace Jasper::Util
{
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
            else OPAL_CORE_ERROR("Could not read from file '{0}'", filepath.string().c_str());

        }
        else OPAL_CORE_ERROR("Could not open file '{0}'", filepath.string().c_str());

        return result;
    }

    void HashCombine(std::size_t& seed, const std::size_t value)
    {
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    }
}
