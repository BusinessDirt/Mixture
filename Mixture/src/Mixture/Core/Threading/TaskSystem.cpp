#include "mxpch.hpp"
#include "Mixture/Core/Threading/TaskSystem.hpp"

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

namespace Mixture
{
    struct TaskQueue
    {
        std::queue<std::function<void()>> Queue;
        std::mutex Mutex;
        std::condition_variable Condition;
        std::atomic<bool> Running = true;
    };

    static TaskQueue s_TaskQueue;
    static std::vector<std::thread> s_Threads;

    void TaskSystem::Init(uint32_t threadCount)
    {
        if (threadCount == 0)
        {
            threadCount = std::thread::hardware_concurrency();
            if (threadCount > 1) threadCount--; // Leave one for main thread
            else threadCount = 1;
        }

        OPAL_INFO("Core/Threading", "Initializing TaskSystem with {} threads.", threadCount);

        s_TaskQueue.Running = true;

        for (uint32_t i = 0; i < threadCount; ++i)
        {
            s_Threads.emplace_back([i]()
            {
                std::string threadName = "Worker Thread " + std::to_string(i);
                Opal::LogRegistry::SetThreadName(threadName);

                while (s_TaskQueue.Running)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(s_TaskQueue.Mutex);
                        
                        s_TaskQueue.Condition.wait(lock, [] { 
                            return !s_TaskQueue.Queue.empty() || !s_TaskQueue.Running; 
                        });

                        if (!s_TaskQueue.Running && s_TaskQueue.Queue.empty())
                            return;

                        if (s_TaskQueue.Queue.empty())
                            continue;

                        task = std::move(s_TaskQueue.Queue.front());
                        s_TaskQueue.Queue.pop();
                    }

                    try
                    {
                        task();
                    }
                    catch (const std::exception& e)
                    {
                        OPAL_ERROR("Core/Threading", "Task threw exception: {}", e.what());
                    }
                    catch (...)
                    {
                        OPAL_ERROR("Core/Threading", "Task threw unknown exception!");
                    }
                }
            });
        }
    }

    void TaskSystem::Shutdown()
    {
        s_TaskQueue.Running = false;
        s_TaskQueue.Condition.notify_all();

        for (auto& thread : s_Threads)
        {
            if (thread.joinable())
                thread.join();
        }

        s_Threads.clear();
        OPAL_INFO("Core/Threading", "TaskSystem Shutdown.");
    }

    void TaskSystem::Submit(std::function<void()> task)
    {
        {
            std::lock_guard<std::mutex> lock(s_TaskQueue.Mutex);
            s_TaskQueue.Queue.push(std::move(task));
        }
        s_TaskQueue.Condition.notify_one();
    }
}
