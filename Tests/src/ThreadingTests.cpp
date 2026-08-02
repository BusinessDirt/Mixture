#include <gtest/gtest.h>
#include "Mixture/Core/Threading/TaskSystem.hpp"
#include <atomic>
#include <thread>
#include <chrono>

namespace Mixture::Tests {

    class TaskSystemTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            TaskSystem::Init(2); // Use 2 threads for testing
        }

        void TearDown() override
        {
            TaskSystem::Shutdown();
        }
    };

    TEST_F(TaskSystemTests, SimpleSubmission)
    {
        std::atomic<bool> executed = false;
        
        TaskSystem::Submit([&]() {
            executed = true;
        });

        // Wait a bit for execution
        int timeout = 100;
        while (!executed && timeout-- > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        EXPECT_TRUE(executed);
    }

    TEST_F(TaskSystemTests, FutureResult)
    {
        auto future = TaskSystem::SubmitFuture([]() {
            return 42;
        });

        // This should block until ready
        int result = future.get();
        EXPECT_EQ(result, 42);
    }

    TEST_F(TaskSystemTests, MultipleTasks)
    {
        const int count = 100;
        std::atomic<int> counter = 0;

        for (int i = 0; i < count; ++i)
        {
            TaskSystem::Submit([&]() {
                counter++;
            });
        }

        // Wait
        int timeout = 500; // 5 seconds max
        while (counter < count && timeout-- > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        EXPECT_EQ(counter, count);
    }

    TEST_F(TaskSystemTests, ThreadCheck)
    {
        std::thread::id mainThreadId = std::this_thread::get_id();
        
        auto future = TaskSystem::SubmitFuture([mainThreadId]() {
            return std::this_thread::get_id() != mainThreadId;
        });

        EXPECT_TRUE(future.get());
    }

    TEST_F(TaskSystemTests, LifecycleIsIdempotent)
    {
        ASSERT_TRUE(TaskSystem::IsInitialized());

        TaskSystem::Init(2);
        EXPECT_TRUE(TaskSystem::IsInitialized());

        TaskSystem::Shutdown();
        EXPECT_FALSE(TaskSystem::IsInitialized());

        TaskSystem::Shutdown();
        TaskSystem::Init(2);
        EXPECT_TRUE(TaskSystem::IsInitialized());
    }

}
