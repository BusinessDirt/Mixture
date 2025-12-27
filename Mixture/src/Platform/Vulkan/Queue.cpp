#include "mxpch.hpp"
#include "Platform/Vulkan/Queue.hpp"

namespace Mixture::Vulkan
{
    Queue::Queue(vk::Device device, std::optional<uint32_t> queueIndex,
                 std::string_view debugName, vk::Queue fallback)
        : m_DebugName(debugName)
    {
        if (queueIndex.has_value())
        {
            m_Handle = device.getQueue(queueIndex.value(), 0);
            return;
        }

        if (fallback == nullptr)
        {
            OPAL_WARN("Core/Vulkan", "Tried to create '{}' but index had no value and fallback is nullptr",
                      m_DebugName);
        }

        m_Handle = fallback;
    }

    void Queue::Submit(Vector<vk::CommandBuffer> commandBuffers,
                       Vector<vk::Semaphore> signalSemaphores,
                       Vector<vk::Semaphore> waitSemaphores,
                       Vector<vk::PipelineStageFlags> waitStages,
                       vk::Fence fence)
    {
        if (waitSemaphores.size() != waitStages.size())
        {
            OPAL_ERROR("Core/Vulkan", "Size of waitSemaphores={} and waitStages={} must be equal.",
                       waitSemaphores.size(), waitStages.size());
            return;
        }

        try
        {
            vk::SubmitInfo submitInfo;
            submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
            submitInfo.pCommandBuffers = commandBuffers.data();
            submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
            submitInfo.pSignalSemaphores = signalSemaphores.data();
            submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
            submitInfo.pWaitSemaphores = waitSemaphores.data();
            submitInfo.pWaitDstStageMask = waitStages.data();
            m_Handle.submit(submitInfo, fence);
        }
        catch (vk::SystemError& err)
        {
            OPAL_CRITICAL("Core/Vulkan", "Submitting '{}' failed: {}", m_DebugName, err.what());
        }
    }
}
