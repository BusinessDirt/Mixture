#pragma once

/**
 * @file SingleTimeCommand.hpp
 * @brief Helper for executing single-use Vulkan commands.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Queue.hpp"
#include <future>

namespace Mixture::Vulkan
{
    /**
     * @brief Batches short-lived upload commands on a reusable asynchronous worker.
     */
    class SingleTimeCommand
    {
    public:
        struct Statistics
        {
            uint64_t UploadCount = 0;
            uint64_t BatchCount = 0;
            uint64_t CommandBufferCount = 0;
            uint64_t FenceCount = 0;
        };

        using Completion = std::shared_future<void>;

        static Completion Submit(const std::function<void(vk::CommandBuffer)>& action,
            std::function<void()> cleanup = {});

        /** @brief Queues an upload command and optional post-completion cleanup. */
        static Completion Submit(Queue& queue, const std::function<void(vk::CommandBuffer)>& action,
            std::function<void()> cleanup = {});

        /** @brief Ensures all currently queued uploads have been submitted. */
        static void Flush(Queue& queue);

        /** @brief Returns counters for validating batching and synchronization reuse. */
        static Statistics GetStatistics(Queue& queue);

        /** @brief Drains and destroys the reusable service for a queue. */
        static void Shutdown(Queue& queue);
    };
}
