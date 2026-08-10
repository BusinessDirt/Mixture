#pragma once

/**
 * @file Definitions.hpp
 * @brief Common definitions and structures for Vulkan.
 */

#include <vulkan/vulkan.hpp>
#include <Opal/Formatters/VulkanFormatters.hpp>

#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/EnumMapper.hpp"

#include <optional>
#include <array>
#include <algorithm>

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

    inline Vector<uint32_t> CollectQueueFamilyIndices(const QueueFamilyIndices& indices)
    {
        Vector<uint32_t> result;
        for (const auto& index : std::array{ indices.Graphics, indices.Present, indices.Transfer, indices.Compute })
            if (index && std::find(result.begin(), result.end(), *index) == result.end()) result.push_back(*index);
        std::sort(result.begin(), result.end());
        return result;
    }
}
