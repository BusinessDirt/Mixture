#include "mxpch.hpp"
#include "IndexBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    IndexBuffer::IndexBuffer(const std::vector<uint16_t>& indices)
    {
        m_IndexCount = static_cast<uint32_t>(indices.size());
        if (m_IndexCount == 0) return;

        uint32_t indexSize = sizeof(indices[0]);
        VkDeviceSize bufferSize = indexSize * m_IndexCount;

        Buffer stagingBuffer = Buffer(indexSize, m_IndexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer((void*)indices.data());

        m_IndexBuffer = CreateScope<Buffer>(indexSize, m_IndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        Buffer::Copy(stagingBuffer.GetHandle(), m_IndexBuffer->GetHandle(), bufferSize);
    }

    IndexBuffer::~IndexBuffer()
    {
        m_IndexBuffer = nullptr;
    }

    void IndexBuffer::Bind(CommandBuffer commandBuffer)
    {
        vkCmdBindIndexBuffer(commandBuffer.GetAsVulkanHandle(), m_IndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT16);
    }
}
