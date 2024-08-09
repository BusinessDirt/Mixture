#include "mxpch.hpp"
#include "Util.hpp"

namespace Mixture
{
    bool Contains(const std::string& str, const std::string& substr)
    {
        return str.find(substr) != std::string::npos;
    }
}
