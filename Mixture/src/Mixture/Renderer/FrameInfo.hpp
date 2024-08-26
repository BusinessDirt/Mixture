#pragma once

#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    struct FrameInfo
    {
        uint32_t FrameIndex;
        float FrameTime;
        CommandBuffer CommandBuffer;

        VkDescriptorSet GlobalSet; // TODO: abstract
        VkDescriptorSet InstanceSet; // TODO: abstract
    };
}
