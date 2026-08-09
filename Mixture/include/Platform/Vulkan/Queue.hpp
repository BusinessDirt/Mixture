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
        /**
         * @brief Constructs a Vulkan Queue wrapper.
         * 
         * @param device Reference to the logical device.
         * @param queueIndex The index of the queue family.
         * @param frameCount Number of frames in flight (for command buffer allocation).
         * @param debugName Debug name for the queue.
         * @param fallbackIndex Optional fallback queue family index.
         */
        Queue(Device& device, std::optional<uint32_t> queueIndex,
            uint32_t frameCount, std::string_view debugName,
            std::optional<uint32_t> fallbackIndex = std::optional<uint32_t>());
        ~Queue() = default;

        /**
         * @brief Gets the Vulkan Queue handle.
         * 
         * @return vk::Queue The raw handle.
         */
        vk::Queue GetHandle() const { return m_Handle; }
        Device& GetDevice() const { return *m_Device; }
        uint32_t GetFamilyIndex() const { return m_FamilyIndex; }

        /**
         * @brief Resets the command buffer for a specific frame index.
         * 
         * @param frameIndex The frame index.
         */
        void ResetBuffer(uint32_t frameIndex) { m_Buffers->Reset(frameIndex); }

        /**
         * @brief Gets the command buffer for a specific frame index.
         * 
         * @param frameIndex The frame index.
         * @return vk::CommandBuffer The command buffer.
         */
        vk::CommandBuffer GetBuffer(uint32_t frameIndex) { return m_Buffers->Get(frameIndex); }

        /**
         * @brief Gets the command pool associated with this queue.
         * 
         * @return CommandPool& Reference to the command pool.
         */
        CommandPool& GetPool() const { return *m_Pool; }

        /**
         * @brief Submits work to the queue.
         * 
         * @param frameIndex The current frame index (to select the command buffer).
         * @param signalSemaphores Semaphores to signal when execution completes.
         * @param waitSemaphores Semaphores to wait on before execution begins.
         * @param waitStages Pipeline stages to wait at.
         * @param fence Fence to signal when execution completes.
         */
        void Submit(uint32_t frameIndex, Vector<vk::Semaphore> signalSemaphores,
                    Vector<vk::Semaphore> waitSemaphores = {}, Vector<vk::PipelineStageFlags> waitStages = {},
                    vk::Fence fence = {});

    private:
        Device* m_Device;
        vk::Queue m_Handle;
        std::string_view m_DebugName;
        uint32_t m_FamilyIndex = 0;

        Scope<CommandPool> m_Pool;
        Scope<CommandBuffers> m_Buffers;
    };
}
