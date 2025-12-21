#include "mxpch.hpp"
#include "Platform/Vulkan/Command/Buffers.hpp"

namespace Mixture::Vulkan
{
    CommandBuffers::CommandBuffers(vk::Device device, vk::CommandPool commandPool, uint32_t count)
        : m_Device(device), m_CommandPool(commandPool)
    {
        // Allocate Command Buffers (One per frame in flight)
        m_Handles.resize(count);

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = count;

        m_Handles = m_Device.allocateCommandBuffers(allocInfo);
    }

    void CommandBuffers::Reset(uint32_t index)
    {
        m_Handles[index].reset();
    }
}
