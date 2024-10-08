#include "mxpch.hpp"
#include "CommandBuffer.hpp"

namespace Mixture
{
    CommandBuffer::CommandBuffer()
        : m_Handle(nullptr) {}

    CommandBuffer::CommandBuffer(void* handle)
        : m_Handle(handle) {}

    VkCommandBuffer CommandBuffer::GetAsVulkanHandle() const
    {
        // assume its a vulkan handle
        // should not be able to be used without using the vulkan API
        return static_cast<VkCommandBuffer>(m_Handle);
    }
}
