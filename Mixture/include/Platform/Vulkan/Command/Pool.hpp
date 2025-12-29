#pragma once

/**
 * @file Pool.hpp
 * @brief Vulkan Command Pool wrapper.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Wrapper around a Vulkan Command Pool.
     *
     * Manages the creation and destruction of a command pool, used to allocate command buffers.
     */
    class CommandPool
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param device Reference to the logical device.
         * @param indices Queue family indices to determine which queue family the pool supports.
         */
        CommandPool(Device& device, uint32_t queueIndex);
        ~CommandPool();

        /**
         * @brief Gets the Vulkan Command Pool handle.
         *
         * @return vk::CommandPool The command pool handle.
         */
        vk::CommandPool GetHandle() const { return m_Handle; }
    private:
        Device* m_Device;
        vk::CommandPool m_Handle;
    };
}
