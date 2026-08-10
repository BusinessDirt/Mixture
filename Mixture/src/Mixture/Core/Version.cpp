#include "mxpch.hpp"
#include "Mixture/Core/Version.hpp"

namespace Mixture
{
    Version GetVersion()
    {
        return {
            MIXTURE_VERSION_MAJOR,
            MIXTURE_VERSION_MINOR,
            MIXTURE_VERSION_PATCH
        };
    }

    std::string_view GetVersionString()
    {
        return MIXTURE_VERSION;
    }
}
