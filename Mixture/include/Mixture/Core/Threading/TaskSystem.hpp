#pragma once

/**
 * @file TaskSystem.hpp
 * @brief Simple task system for executing code on background threads.
 */

#include "Mixture/Core/Base.hpp"

#include <functional>
#include <future>

namespace Mixture
{
    /**
     * @brief A system to manage background worker threads.
     */
    class TaskSystem
    {
    public:
        /**
         * @brief Initializes the task system with a specific number of threads.
         * 
         * @param threadCount Number of threads. 0 means hardware concurrency - 1.
         */
        static void Init(uint32_t threadCount = 0);

        /**
         * @brief Shuts down the task system, waiting for all tasks to finish (or cancelling them).
         */
        static void Shutdown();

        /**
         * @brief Submits a task to be executed asynchronously.
         * 
         * @param task The function to execute.
         */
        static void Submit(std::function<void()> task);

        /**
         * @brief Submits a task and returns a future to wait for the result.
         * 
         * @tparam F Function type.
         * @tparam Args Argument types.
         * @param f Function to execute.
         * @param args Arguments.
         * @return std::future<Result> The future result.
         */
        template<typename F, typename... Args>
        static auto SubmitFuture(F&& f, Args&&... args) 
            -> std::future<typename std::invoke_result<F, Args...>::type>
        {
            using ReturnType = typename std::invoke_result<F, Args...>::type;

            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<ReturnType> res = task->get_future();
            
            Submit([task]() { (*task)(); });
            
            return res;
        }
    };
}
