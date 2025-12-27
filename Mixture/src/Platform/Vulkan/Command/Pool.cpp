#include "mxpch.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

namespace Mixture::Vulkan
{
    CommandPool::CommandPool(Device& device, const QueueFamilyIndices& indices)
        : m_Device(&device)
    {
        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer; // Allow resetting individual buffers
        poolInfo.queueFamilyIndex = indices.Graphics.value();

        try
        {
            m_Handle = m_Device->GetHandle().createCommandPool(poolInfo);
        }
        catch (vk::SystemError& err)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to create Command Pool!");
            exit(-1);
        }
    }

    CommandPool::~CommandPool()
    {
        m_Device->GetHandle().destroyCommandPool(m_Handle);
    }
}
