#pragma once
#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.hpp>
#include <optional>

namespace Mixture::Vulkan
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Present;
        std::optional<uint32_t> Compute;

        bool IsComplete()
        {
            return Graphics.has_value()
                //&& Present.has_value()
                && Compute.has_value();
        }
    };
}
