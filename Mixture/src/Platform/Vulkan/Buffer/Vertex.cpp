#include "mxpch.hpp"
#include "Platform/Vulkan/Buffer/Vertex.hpp"

#include "Platform/Vulkan/Command/SingleTime.hpp"

namespace Mixture::Vulkan
{
    VertexBuffer::VertexBuffer(const void* data, const uint32_t size, const uint32_t count)
        : m_VertexBuffer(VK_NULL_HANDLE)
    {
        if (count != 0) VertexBuffer::SetData(data, size, count);
    }

    void VertexBuffer::SetData(const void* data, const uint32_t size, const uint32_t count)
    {
        const bool validCommandBuffer = Context::CurrentCommandBuffer != VK_NULL_HANDLE;
        const VkCommandBuffer commandBuffer = validCommandBuffer ? Context::CurrentCommandBuffer : SingleTimeCommand::Begin();
        
        m_VertexCount = count;
        OPAL_CORE_ASSERT(m_VertexCount >= 3, "Mixture::Vulkan::VertexBuffer::SetData() - Vertex count must be at least 3!")

        const VkDeviceSize bufferSize = static_cast<VkDeviceSize>(size) * m_VertexCount;

        // Create a staging buffer for data transfer
        auto stagingBuffer = Buffer(size, m_VertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // Map and write data to the staging buffer
        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(data);

        // If the vertex buffer hasn't been created yet, create it
        if (!m_VertexBuffer || m_VertexBuffer->GetBufferSize() < bufferSize)
        {
            m_VertexBuffer = CreateScope<Buffer>(size, m_VertexCount,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }

        // Copy data from the staging buffer to the GPU vertex buffer
        Buffer::Copy(stagingBuffer.GetHandle(), m_VertexBuffer->GetHandle(), bufferSize, commandBuffer);

        if (!validCommandBuffer) SingleTimeCommand::End(commandBuffer);
    }

    void VertexBuffer::Bind() const
    {
        const VkBuffer buffers[] = { m_VertexBuffer->GetHandle() };
        constexpr VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(Context::CurrentCommandBuffer, 0, 1, buffers, offsets);
    }
}
