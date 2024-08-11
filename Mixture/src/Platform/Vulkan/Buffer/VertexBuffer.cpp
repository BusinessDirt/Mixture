#include "mxpch.hpp"
#include "VertexBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices)
    {
        m_VertexCount = static_cast<uint32_t>(vertices.size());
        MX_CORE_ASSERT(m_VertexCount >= 3, "Vertex count must be at least 3");

        uint32_t vertexSize = sizeof(vertices[0]);
        VkDeviceSize bufferSize = vertexSize * m_VertexCount;

        Buffer stagingBuffer = Buffer(vertexSize, m_VertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer((void*)vertices.data());

        m_VertexBuffer = CreateScope<Buffer>(vertexSize, m_VertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        Buffer::Copy(stagingBuffer.GetHandle(), m_VertexBuffer->GetHandle(), bufferSize);
    }

    VertexBuffer::~VertexBuffer()
    {
        m_VertexBuffer = nullptr;
    }

    void VertexBuffer::Bind(CommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { m_VertexBuffer->GetHandle()};
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer.GetAsVulkanHandle(), 0, 1, buffers, offsets);
    }
}
