#pragma once

/**
 * @file Buffers.hpp
 * @brief Vulkan Command Buffers wrapper.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Wrapper around a collection of Vulkan Command Buffers.
     *
     * Manages allocation and lifecycle of multiple command buffers from a single pool.
     */
    class CommandBuffers
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param device Reference to the logical device.
         * @param commandPool Reference to the command pool to allocate from.
         * @param count Number of command buffers to allocate.
         */
        CommandBuffers(Device& device, CommandPool& commandPool, uint32_t count);
        ~CommandBuffers() = default;

        /**
         * @brief Gets a command buffer at the specified index.
         *
         * @param index Index of the command buffer.
         * @return vk::CommandBuffer The command buffer handle.
         */
        vk::CommandBuffer Get(uint32_t index) const { return m_Handles[index]; }

        /**
         * @brief Gets a pointer to a command buffer at the specified index.
         *
         * @param index Index of the command buffer.
         * @return vk::CommandBuffer* Pointer to the command buffer handle.
         */
        vk::CommandBuffer* GetPointer(uint32_t index) { return &m_Handles[index]; }

        /**
         * @brief Resets the command buffer at the specified index.
         *
         * @param index Index of the command buffer to reset.
         */
        void Reset(uint32_t index);

    private:
        Device* m_Device;
        CommandPool* m_CommandPool;

        Vector<vk::CommandBuffer> m_Handles;
    };
}
