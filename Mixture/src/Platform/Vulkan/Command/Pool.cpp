#include "mxpch.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

namespace Mixture::Vulkan
{
    CommandPool::CommandPool(vk::Device device, const QueueFamilyIndices& indices)
        : m_Device(device)
    {
        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer; // Allow resetting individual buffers
        poolInfo.queueFamilyIndex = indices.Graphics.value();

        try
        {
            m_Handle = m_Device.createCommandPool(poolInfo);
        }
        catch (vk::SystemError& err)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to create Command Pool!");
            exit(-1);
        }
    }

    CommandPool::~CommandPool()
    {
        m_Device.destroyCommandPool(m_Handle);
    }
}
