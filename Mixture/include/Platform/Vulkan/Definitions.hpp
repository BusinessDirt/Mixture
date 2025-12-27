#pragma once

/**
 * @file Definitions.hpp
 * @brief Common definitions and structures for Vulkan.
 */

#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/EnumMapper.hpp"

#include <vulkan/vulkan.hpp>
#include <optional>

namespace Mixture::Vulkan
{
    /**
     * @brief Structure holding indices of queue families.
     */
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Present;
        std::optional<uint32_t> Transfer;
        std::optional<uint32_t> Compute;

        /**
         * @brief Checks if all required queue families are present.
         *
         * @return true If complete.
         */
        bool IsComplete()
        {
            return Graphics.has_value()
                && Present.has_value()
                && Transfer.has_value()
                && Compute.has_value();
        }
    };
}
