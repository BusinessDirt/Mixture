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
    /**
     * @brief Represents the semantic version of the Mixture engine.
     *
     * Each component corresponds to the `MAJOR.MINOR.PATCH` version stored in
     * the repository's `VERSION` file when the engine project is generated.
     */
    struct Version
    {
        /** @brief Major version, incremented for incompatible API changes. */
        uint32_t Major;

        /** @brief Minor version, incremented for backward-compatible features. */
        uint32_t Minor;

        /** @brief Patch version, incremented for backward-compatible fixes. */
        uint32_t Patch;
    };

    /**
     * @brief Gets the semantic version of the Mixture engine.
     *
     * @return The engine version as separate major, minor, and patch values.
     */
    OPAL_NODISCARD Version GetVersion();

    /**
     * @brief Gets the complete Mixture engine version string.
     *
     * @return A non-owning view of the version in `MAJOR.MINOR.PATCH` format.
     */
    OPAL_NODISCARD std::string_view GetVersionString();
}
