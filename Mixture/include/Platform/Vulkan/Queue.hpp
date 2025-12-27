#pragma once
#include "Platform/Vulkan/Definitions.hpp"

#include <optional>

namespace Mixture::Vulkan
{
    class Queue
    {
    public:
        Queue(vk::Device device, std::optional<uint32_t> queueIndex,
            std::string_view debugName, vk::Queue fallback = {});
        ~Queue() = default;

        vk::Queue GetHandle() const { return m_Handle; }
        void Submit(Vector<vk::CommandBuffer> commandBuffers,
                    Vector<vk::Semaphore> signalSemaphores,
                    Vector<vk::Semaphore> waitSemaphores = {},
                    Vector<vk::PipelineStageFlags> waitStages = {},
                    vk::Fence fence = {});

    private:
        vk::Queue m_Handle;
        std::string_view m_DebugName;
    };
}
