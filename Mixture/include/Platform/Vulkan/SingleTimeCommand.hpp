#pragma once

/**
 * @file SingleTimeCommand.hpp
 * @brief Helper for executing single-use Vulkan commands.
 */

#include "Platform/Vulkan/Definitions.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Helper class to create and submit short-lived command buffers.
     *
     * Useful for resource transfers (staging buffers) or layout transitions during initialization.
     */
    class SingleTimeCommand
    {
    public:
        /**
         * @brief Begins a single-time command buffer.
         *
         * @return vk::CommandBuffer The recorded command buffer.
         */
        static vk::CommandBuffer Begin();

        /**
         * @brief Ends and submits the single-time command buffer, waiting for completion.
         *
         * @param commandBuffer The command buffer to submit.
         */
        static void End(const vk::CommandBuffer commandBuffer);

        /**
         * @brief Convenience function to run a lambda within a single-time command buffer.
         *
         * @param action Lambda taking the command buffer to record commands into.
         */
        static void Submit(const std::function<void(vk::CommandBuffer)>& action);
    };
}
