#pragma once

/**
 * @file Semaphores.hpp
 * @brief Vulkan Semaphores wrapper.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Wrapper around a collection of Vulkan Semaphores.
     * 
     * Semaphores are used for GPU-GPU synchronization (e.g., synchronizing queue submissions).
     */
    class Semaphores
    {
    public:
        /**
         * @brief Constructor.
         * 
         * @param device Reference to the logical device.
         * @param count Number of semaphores to create.
         */
        Semaphores(Device& device, uint32_t count);
        ~Semaphores();

        /**
         * @brief Gets a semaphore at the specified index.
         * 
         * @param index Index of the semaphore.
         * @return vk::Semaphore The semaphore handle.
         */
        vk::Semaphore Get(uint32_t index) const { return m_Handles[index]; }
    private:
        Device* m_Device;
        Vector<vk::Semaphore> m_Handles;
    };
}
