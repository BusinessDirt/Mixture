#include "mxpch.hpp"
#include "Platform/Vulkan/SingleTimeCommand.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Device.hpp"

#include <condition_variable>
#include <deque>
#include <thread>
#include <unordered_map>

namespace Mixture::Vulkan
{
    namespace
    {
        struct UploadRequest
        {
            std::function<void(vk::CommandBuffer)> Record;
            std::function<void()> Cleanup;
            std::promise<void> Completed;
            uint64_t Serial = 0;
        };

        class UploadService
        {
        public:
            explicit UploadService(Queue& queue)
                : m_Queue(queue), m_Device(queue.GetDevice())
            {
                vk::CommandPoolCreateInfo poolInfo;
                poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
                poolInfo.queueFamilyIndex = queue.GetFamilyIndex();
                m_CommandPool = m_Device.GetHandle().createCommandPool(poolInfo);

                vk::CommandBufferAllocateInfo allocationInfo;
                allocationInfo.commandPool = m_CommandPool;
                allocationInfo.level = vk::CommandBufferLevel::ePrimary;
                allocationInfo.commandBufferCount = 1;
                m_CommandBuffer = m_Device.GetHandle().allocateCommandBuffers(allocationInfo).front();
                m_Fence = m_Device.GetHandle().createFence({});
                m_Statistics.CommandBufferCount = 1;
                m_Statistics.FenceCount = 1;
                m_Worker = std::thread(&UploadService::Run, this);
            }

            ~UploadService()
            {
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    m_Running = false;
                }
                m_Condition.notify_one();
                if (m_Worker.joinable()) m_Worker.join();
                m_Device.GetHandle().destroyFence(m_Fence);
                m_Device.GetHandle().destroyCommandPool(m_CommandPool);
            }

            SingleTimeCommand::Completion Enqueue(std::function<void(vk::CommandBuffer)> record,
                std::function<void()> cleanup)
            {
                auto request = std::make_shared<UploadRequest>();
                request->Record = std::move(record);
                request->Cleanup = std::move(cleanup);
                SingleTimeCommand::Completion completion = request->Completed.get_future().share();
                {
                    std::lock_guard<std::mutex> lock(m_Mutex);
                    request->Serial = ++m_LastEnqueuedSerial;
                    m_Pending.push_back(std::move(request));
                    ++m_Statistics.UploadCount;
                }
                m_Condition.notify_one();
                return completion;
            }

            void Flush()
            {
                std::unique_lock<std::mutex> lock(m_Mutex);
                const uint64_t targetSerial = m_LastEnqueuedSerial;
                m_SubmissionCondition.wait(lock, [this, targetSerial]
                {
                    return m_LastSubmittedSerial >= targetSerial;
                });
            }

            SingleTimeCommand::Statistics GetStatistics()
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return m_Statistics;
            }

        private:
            void Run()
            {
                Opal::LogRegistry::SetThreadName("GPU Upload");
                while (true)
                {
                    std::deque<std::shared_ptr<UploadRequest>> batch;
                    {
                        std::unique_lock<std::mutex> lock(m_Mutex);
                        m_Condition.wait(lock, [this] { return !m_Pending.empty() || !m_Running; });
                        if (m_Pending.empty() && !m_Running) break;
                        m_Condition.wait_for(lock, std::chrono::milliseconds(1));
                        batch.swap(m_Pending);
                        ++m_Statistics.BatchCount;
                    }

                    try
                    {
                        m_Device.GetHandle().resetFences(m_Fence);
                        m_CommandBuffer.reset();
                        vk::CommandBufferBeginInfo beginInfo;
                        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
                        m_CommandBuffer.begin(beginInfo);
                        for (const auto& request : batch) request->Record(m_CommandBuffer);
                        m_CommandBuffer.end();

                        vk::SubmitInfo submitInfo;
                        submitInfo.commandBufferCount = 1;
                        submitInfo.pCommandBuffers = &m_CommandBuffer;
                        m_Device.Submit(m_Queue.GetHandle(), submitInfo, m_Fence);
                        {
                            std::lock_guard<std::mutex> lock(m_Mutex);
                            m_LastSubmittedSerial = batch.back()->Serial;
                        }
                        m_SubmissionCondition.notify_all();

                        const vk::Result result = m_Device.GetHandle().waitForFences(m_Fence, VK_TRUE, UINT64_MAX);
                        if (result != vk::Result::eSuccess)
                            throw std::runtime_error("Batched GPU upload fence wait failed");
                        for (const auto& request : batch)
                        {
                            if (request->Cleanup) request->Cleanup();
                            request->Completed.set_value();
                        }
                    }
                    catch (...)
                    {
                        const std::exception_ptr error = std::current_exception();
                        OPAL_ERROR("Core/Vulkan", "Batched GPU upload failed.");
                        {
                            std::lock_guard<std::mutex> lock(m_Mutex);
                            m_LastSubmittedSerial = batch.back()->Serial;
                        }
                        m_SubmissionCondition.notify_all();
                        for (const auto& request : batch)
                        {
                            if (request->Cleanup) request->Cleanup();
                            request->Completed.set_exception(error);
                        }
                    }
                }
            }

            Queue& m_Queue;
            Device& m_Device;
            vk::CommandPool m_CommandPool;
            vk::CommandBuffer m_CommandBuffer;
            vk::Fence m_Fence;
            std::mutex m_Mutex;
            std::condition_variable m_Condition;
            std::condition_variable m_SubmissionCondition;
            std::deque<std::shared_ptr<UploadRequest>> m_Pending;
            bool m_Running = true;
            std::thread m_Worker;
            SingleTimeCommand::Statistics m_Statistics;
            uint64_t m_LastEnqueuedSerial = 0;
            uint64_t m_LastSubmittedSerial = 0;
        };

        std::mutex s_ServicesMutex;
        std::unordered_map<Queue*, std::unique_ptr<UploadService>> s_Services;

        UploadService& GetService(Queue& queue)
        {
            std::lock_guard<std::mutex> lock(s_ServicesMutex);
            auto [it, inserted] = s_Services.try_emplace(&queue);
            if (inserted) it->second = std::make_unique<UploadService>(queue);
            return *it->second;
        }
    }

    SingleTimeCommand::Completion SingleTimeCommand::Submit(
        const std::function<void(vk::CommandBuffer)>& action, std::function<void()> cleanup)
    {
        return Submit(Context::Get().GetGraphicsQueue(), action, std::move(cleanup));
    }

    SingleTimeCommand::Completion SingleTimeCommand::Submit(Queue& queue,
        const std::function<void(vk::CommandBuffer)>& action, std::function<void()> cleanup)
    {
        return GetService(queue).Enqueue(action, std::move(cleanup));
    }

    void SingleTimeCommand::Flush(Queue& queue)
    {
        UploadService* service = nullptr;
        {
            std::lock_guard<std::mutex> lock(s_ServicesMutex);
            auto it = s_Services.find(&queue);
            if (it == s_Services.end()) return;
            service = it->second.get();
        }
        service->Flush();
    }

    void SingleTimeCommand::Shutdown(Queue& queue)
    {
        std::unique_ptr<UploadService> service;
        {
            std::lock_guard<std::mutex> lock(s_ServicesMutex);
            auto it = s_Services.find(&queue);
            if (it == s_Services.end()) return;
            service = std::move(it->second);
            s_Services.erase(it);
        }
    }

    SingleTimeCommand::Statistics SingleTimeCommand::GetStatistics(Queue& queue)
    {
        std::lock_guard<std::mutex> lock(s_ServicesMutex);
        auto it = s_Services.find(&queue);
        return it == s_Services.end() ? Statistics{} : it->second->GetStatistics();
    }
}
