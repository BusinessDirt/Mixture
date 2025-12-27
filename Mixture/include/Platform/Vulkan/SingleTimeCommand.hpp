#pragma once

/**
 * @file SingleTimeCommand.hpp
 * @brief Helper for executing single-use Vulkan commands.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Queue.hpp"

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
         * @brief Begins a single-time command buffer on the default graphics queue.
         *
         * @return vk::CommandBuffer The recorded command buffer.
         */
        static vk::CommandBuffer Begin();

        /**
         * @brief Begins a single-time command buffer using a specific pool.
         * 
         * @param commandPool The command pool to allocate from.
         * @return vk::CommandBuffer The recorded command buffer.
         */
        static vk::CommandBuffer Begin(vk::CommandPool commandPool);

        /**
         * @brief Ends and submits the single-time command buffer to the default graphics queue, waiting for completion.
         *
         * @param commandBuffer The command buffer to submit.
         */
        static void End(const vk::CommandBuffer commandBuffer);

        /**
         * @brief Ends and submits the command buffer to a specific queue, waiting for completion.
         * 
         * @param commandBuffer The command buffer to submit.
         * @param queue The queue to submit to.
         * @param commandPool The pool the buffer was allocated from (for freeing).
         */
        static void End(vk::CommandBuffer commandBuffer, Queue& queue, vk::CommandPool commandPool);

        /**
         * @brief Convenience function to run a lambda within a single-time command buffer on the default graphics queue.
         *
         * @param action Lambda taking the command buffer to record commands into.
         */
        static void Submit(const std::function<void(vk::CommandBuffer)>& action);

        /**
         * @brief Convenience function to run a lambda within a single-time command buffer on a specific queue.
         * 
         * @param queue The queue to submit to.
         * @param commandPool The command pool to allocate from.
         * @param action Lambda taking the command buffer to record commands into.
         */
        static void Submit(Queue& queue, vk::CommandPool commandPool, const std::function<void(vk::CommandBuffer)>& action);
    };
}
