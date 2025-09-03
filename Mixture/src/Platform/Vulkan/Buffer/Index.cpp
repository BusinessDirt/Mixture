#include "mxpch.hpp"
#include "Platform/Vulkan/Buffer/Index.hpp"

#include "Platform/Vulkan/Command/SingleTime.hpp"

namespace Mixture::Vulkan
{
    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices)
    {
        IndexBuffer::SetData(indices);
    }

    IndexBuffer::IndexBuffer() : m_IndexBuffer(VK_NULL_HANDLE) {}

    IndexBuffer::~IndexBuffer()
    {
        m_IndexBuffer = nullptr;
    }

    void IndexBuffer::SetData(const std::vector<uint32_t>& indices)
    {
        const bool validCommandBuffer = Context::CurrentCommandBuffer != VK_NULL_HANDLE;
        const VkCommandBuffer commandBuffer = validCommandBuffer ? Context::CurrentCommandBuffer : SingleTimeCommand::Begin();
        
        m_IndexCount = static_cast<uint32_t>(indices.size());
        if (m_IndexCount == 0) return;

        uint32_t indexSize = sizeof(indices[0]);
        const VkDeviceSize bufferSize = static_cast<VkDeviceSize>(m_IndexCount) * indexSize;

        auto stagingBuffer = Buffer(indexSize, m_IndexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(indices.data());

        m_IndexBuffer = CreateScope<Buffer>(indexSize, m_IndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Buffer::Copy(stagingBuffer.GetHandle(), m_IndexBuffer->GetHandle(), bufferSize, commandBuffer);

        if (!validCommandBuffer) SingleTimeCommand::End(commandBuffer);
    }

    void IndexBuffer::Bind() const
    {
        vkCmdBindIndexBuffer(Context::CurrentCommandBuffer, m_IndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
    }
}
