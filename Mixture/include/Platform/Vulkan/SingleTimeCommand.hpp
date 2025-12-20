#pragma once
#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.hpp>

namespace Mixture::Vulkan
{
    class SingleTimeCommand
    {
    public:
        static vk::CommandBuffer Begin();
        static void End(const vk::CommandBuffer commandBuffer);
        static void Submit(const std::function<void(vk::CommandBuffer)>& action);
    };
}
