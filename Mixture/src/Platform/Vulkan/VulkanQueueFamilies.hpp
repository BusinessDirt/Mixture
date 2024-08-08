#pragma once

#include "Mixture/Core/Base.hpp"

#include <optional>
#include <vulkan/vulkan.h>

namespace Mixture
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Present;
        std::optional<uint32_t> Compute;
        
        bool IsComplete();
    };

    class VulkanQueueFamilies
    {
    public:
        static QueueFamilyIndices Find(VkPhysicalDevice device);
    };
}
