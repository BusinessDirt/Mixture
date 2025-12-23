#pragma once

/**
 * @file Fences.hpp
 * @brief Vulkan Fences wrapper.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Wrapper around a collection of Vulkan Fences.
     * 
     * Fences are used for CPU-GPU synchronization (e.g., waiting for a frame to finish rendering).
     */
    class Fences
    {
    public:
        /**
         * @brief Constructor.
         * 
         * @param device Reference to the logical device.
         * @param count Number of fences to create.
         * @param signaled Whether to create the fences in a signaled state.
         */
        Fences(Device& device, uint32_t count, bool signaled);
        ~Fences();

        /**
         * @brief Gets a fence at the specified index.
         * 
         * @param index Index of the fence.
         * @return vk::Fence The fence handle.
         */
        vk::Fence Get(uint32_t index) const { return m_Handles[index]; }

        /**
         * @brief Waits for the fence at the specified index to be signaled.
         * 
         * @param index Index of the fence.
         * @return vk::Result The result of the wait operation.
         */
        vk::Result Wait(uint32_t index);

        /**
         * @brief Resets the fence at the specified index to the unsignaled state.
         * 
         * @param index Index of the fence.
         * @return vk::Result The result of the reset operation.
         */
        vk::Result Reset(uint32_t index);
    private:
        Device* m_Device;
        Vector<vk::Fence> m_Handles;
    };
}
