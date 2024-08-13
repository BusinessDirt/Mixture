#include "mxpch.hpp"
#include "Buffer.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Command/SingleTimeCommand.hpp"

namespace Mixture::Vulkan
{
    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0) return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        return instanceSize;
    }

    Buffer::Buffer(VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
        : m_InstanceSize{ instanceSize }, m_InstanceCount{ instanceCount }, m_UsageFlags{ usageFlags }, m_MemoryPropertyFlags{ memoryPropertyFlags }
    {
        m_AlignmentSize = GetAlignment(m_InstanceSize, minOffsetAlignment);
        m_BufferSize = m_AlignmentSize * m_InstanceCount;
        Buffer::Create(m_BufferSize, m_UsageFlags, m_MemoryPropertyFlags, m_Buffer, m_Memory);
    }

    Buffer::~Buffer()
    {
        Unmap();
        vkDestroyBuffer(Context::Get().Device->GetHandle(), m_Buffer, nullptr);
        vkFreeMemory(Context::Get().Device->GetHandle(), m_Memory, nullptr);
    }

    /**
     * Map a memory range of this buffer. If successful, m_Mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset)
    {
        MX_CORE_ASSERT(m_Buffer && m_Memory, "Called map on buffer before create");
        return vkMapMemory(Context::Get().Device->GetHandle(), m_Memory, offset, size, 0, &m_Mapped);
    }

    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void Buffer::Unmap()
    {
        if (m_Mapped)
        {
            vkUnmapMemory(Context::Get().Device->GetHandle(), m_Memory);
            m_Mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
    {
        MX_CORE_ASSERT(m_Mapped, "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(m_Mapped, data, m_BufferSize);
        }
        else
        {
            char* memOffset = (char*)m_Mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange m_MappedRange = {};
        m_MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        m_MappedRange.memory = m_Memory;
        m_MappedRange.offset = offset;
        m_MappedRange.size = size;
        return vkFlushMappedMemoryRanges(Context::Get().Device->GetHandle(), 1, &m_MappedRange);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange m_MappedRange = {};
        m_MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        m_MappedRange.memory = m_Memory;
        m_MappedRange.offset = offset;
        m_MappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(Context::Get().Device->GetHandle(), 1, &m_MappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo Buffer::GetDescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
    {
        return VkDescriptorBufferInfo{ m_Buffer, offset, size, };
    }

    /**
     * Copies "m_InstanceSize" bytes of data to the mapped buffer at an offset of index * m_AlignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void Buffer::WriteToIndex(void* data, int index)
    {
        WriteToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
    }

    /**
     *  Flush the memory range at index * m_AlignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult Buffer::FlushIndex(int index)
    {
        return Flush(m_AlignmentSize, index * m_AlignmentSize);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * m_AlignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo Buffer::GetDescriptorInfoForIndex(int index)
    {
        return GetDescriptorInfo(m_AlignmentSize, index * m_AlignmentSize);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * m_AlignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult Buffer::InvalidateIndex(int index)
    {
        return Invalidate(m_AlignmentSize, index * m_AlignmentSize);
    }

    void Buffer::Create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        MX_VK_ASSERT(vkCreateBuffer(Context::Get().Device->GetHandle(), &bufferInfo, nullptr, &buffer), "Failed to create VkBuffer");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(Context::Get().Device->GetHandle(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Context::Get().Device->FindMemoryType(memRequirements.memoryTypeBits, properties);

        MX_VK_ASSERT(vkAllocateMemory(Context::Get().Device->GetHandle(), &allocInfo, nullptr, &bufferMemory), "Failed to allocate buffer memory");

        vkBindBufferMemory(Context::Get().Device->GetHandle(), buffer, bufferMemory, 0);
    }

    void Buffer::Copy(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        SingleTimeCommand::Submit([&](VkCommandBuffer commandBuffer)
            {
                VkBufferCopy copyRegion{};
                copyRegion.srcOffset = 0;  // Optional
                copyRegion.dstOffset = 0;  // Optional
                copyRegion.size = size;
                vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
            });
    }
}
