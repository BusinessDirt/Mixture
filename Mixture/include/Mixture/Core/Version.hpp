#pragma once

/**
 * @file Version.hpp
 * @brief Manages the version tracking of the Engine.
 */

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <string_view>

namespace Mixture
{
    struct Version
    {
        uint32_t Major;
        uint32_t Minor;
        uint32_t Patch;
    };

    /**
     * @brief
     */
    OPAL_NODISCARD Version GetVersion();

    /**
     * @brief
     */
    OPAL_NODISCARD std::string_view GetVersionString();
}
