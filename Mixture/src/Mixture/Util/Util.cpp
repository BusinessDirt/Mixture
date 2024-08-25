#include "mxpch.hpp"
#include "Util.hpp"

namespace Mixture
{
    bool Contains(const std::string& str, const std::string& substr)
    {
        return str.find(substr) != std::string::npos;
    }

    std::string ReadFile(const std::filesystem::path& filepath) 
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII

        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();

            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
            else MX_CORE_ERROR("Could not read from file '{0}'", filepath.string().c_str());

        }
        else MX_CORE_ERROR("Could not open file '{0}'", filepath.string().c_str());

        return result;
    }
}
