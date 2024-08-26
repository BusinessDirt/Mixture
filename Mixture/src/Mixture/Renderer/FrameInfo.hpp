#pragma once

#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"
#include "Mixture/Core/Timestep.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    struct FrameInfo
    {
        uint32_t FrameIndex;
        const Timestep& FrameTime;
        CommandBuffer CommandBuffer;

        VkDescriptorSet GlobalSet; // TODO: abstract
        VkDescriptorSet InstanceSet; // TODO: abstract
    };
}
