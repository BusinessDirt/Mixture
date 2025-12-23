#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

namespace Mixture::Vulkan
{
    class CommandBuffers
    {
    public:
        CommandBuffers(Device& device, CommandPool& commandPool, uint32_t count);
        ~CommandBuffers() = default;

        vk::CommandBuffer Get(uint32_t index) const { return m_Handles[index]; }
        vk::CommandBuffer* GetPointer(uint32_t index) { return &m_Handles[index]; }

        void Reset(uint32_t index);

    private:
        Device* m_Device;
        CommandPool* m_CommandPool;

        Vector<vk::CommandBuffer> m_Handles;
    };
}
