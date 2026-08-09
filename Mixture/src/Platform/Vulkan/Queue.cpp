#include "mxpch.hpp"
#include "Platform/Vulkan/Queue.hpp"

namespace Mixture::Vulkan
{
    Queue::Queue(Device& device, std::optional<uint32_t> queueIndex,
            uint32_t frameCount, std::string_view debugName,
            std::optional<uint32_t> fallbackIndex)
        : m_Device(&device), m_DebugName(debugName)
    {
        if (!queueIndex.has_value() && !fallbackIndex.has_value())
        {
            OPAL_ERROR("Core/Vulkan", "Tried to create '{}' but index had no value and fallback index has no value.",
                m_DebugName);
            return;
        }

        uint32_t index = queueIndex.has_value() ? queueIndex.value() : fallbackIndex.value();
        m_FamilyIndex = index;
        m_Handle = m_Device->GetHandle().getQueue(index, 0);

        if (frameCount == 0) return;
        m_Pool = CreateScope<CommandPool>(*m_Device, index);
        m_Buffers = CreateScope<CommandBuffers>(*m_Device, *m_Pool, frameCount);
    }

    void Queue::Submit(uint32_t frameIndex,
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
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = m_Buffers->GetPointer(frameIndex);
            submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
            submitInfo.pSignalSemaphores = signalSemaphores.data();
            submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
            submitInfo.pWaitSemaphores = waitSemaphores.data();
            submitInfo.pWaitDstStageMask = waitStages.data();
            m_Device->Submit(m_Handle, submitInfo, fence);
        }
        catch (vk::SystemError& err)
        {
            OPAL_CRITICAL("Core/Vulkan", "Submitting '{}' failed: {}", m_DebugName, err.what());
        }
    }
}
