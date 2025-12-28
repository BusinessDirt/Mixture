#pragma once
#include "Platform/Vulkan/Definitions.hpp"

#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"
#include "Platform/Vulkan/Command/Buffers.hpp"

#include <optional>

namespace Mixture::Vulkan
{
    class Queue
    {
    public:
        Queue(Device& device, std::optional<uint32_t> queueIndex,
            uint32_t frameCount, std::string_view debugName,
            std::optional<uint32_t> fallbackIndex = std::optional<uint32_t>());
        ~Queue() = default;

        vk::Queue GetHandle() const { return m_Handle; }

        void ResetBuffer(uint32_t frameIndex) { m_Buffers->Reset(frameIndex); }
        vk::CommandBuffer GetBuffer(uint32_t frameIndex) { return m_Buffers->Get(frameIndex); }
        CommandPool& GetPool() const { return *m_Pool; }

        void Submit(uint32_t frameIndex, Vector<vk::Semaphore> signalSemaphores,
                    Vector<vk::Semaphore> waitSemaphores = {}, Vector<vk::PipelineStageFlags> waitStages = {},
                    vk::Fence fence = {});

    private:
        Device* m_Device;
        vk::Queue m_Handle;
        std::string_view m_DebugName;

        Scope<CommandPool> m_Pool;
        Scope<CommandBuffers> m_Buffers;
    };
}
