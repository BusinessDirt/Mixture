#include "mxpch.hpp"
#include "Platform/Vulkan/Buffer/Buffer.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Command/SingleTime.hpp"

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
    VkDeviceSize Buffer::GetAlignment(const VkDeviceSize instanceSize, const VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0) return instanceSize + minOffsetAlignment - 1 & ~(minOffsetAlignment - 1);
        return instanceSize;
    }

    Buffer::Buffer(const VkDeviceSize instanceSize, const uint32_t instanceCount, const VkBufferUsageFlags usageFlags,
                   const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize minOffsetAlignment)
        : m_InstanceCount{ instanceCount }, m_InstanceSize{ instanceSize }, m_UsageFlags{ usageFlags }, m_MemoryPropertyFlags{ memoryPropertyFlags }
    {
        m_AlignmentSize = GetAlignment(m_InstanceSize, minOffsetAlignment);
        m_BufferSize = m_AlignmentSize * m_InstanceCount;
        Create(m_BufferSize, m_UsageFlags, m_MemoryPropertyFlags, m_Buffer, m_Memory);
    }

    Buffer::~Buffer()
    {
        Unmap();
        vkDestroyBuffer(Context::Device->GetHandle(), m_Buffer, nullptr);
        vkFreeMemory(Context::Device->GetHandle(), m_Memory, nullptr);
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
    VkResult Buffer::Map(const VkDeviceSize size, const VkDeviceSize offset)
    {
        OPAL_ASSERT("Core", m_Buffer && m_Memory, "Called map on buffer before create")
        return vkMapMemory(Context::Device->GetHandle(), m_Memory, offset, size, 0, &m_Mapped);
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
            vkUnmapMemory(Context::Device->GetHandle(), m_Memory);
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
    void Buffer::WriteToBuffer(const void* data, const VkDeviceSize size, const VkDeviceSize offset) const
    {
        OPAL_ASSERT("Core", m_Mapped, "Mixture::Vulkan::Buffer::WriteToBuffer() - Cannot copy to unmapped buffer")

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(m_Mapped, data, m_BufferSize);
        }
        else
        {
            auto memOffset = static_cast<char*>(m_Mapped);
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
    VkResult Buffer::Flush(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange m_MappedRange = {};
        m_MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        m_MappedRange.memory = m_Memory;
        m_MappedRange.offset = offset;
        m_MappedRange.size = size;
        return vkFlushMappedMemoryRanges(Context::Device->GetHandle(), 1, &m_MappedRange);
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
    VkResult Buffer::Invalidate(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange m_MappedRange = {};
        m_MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        m_MappedRange.memory = m_Memory;
        m_MappedRange.offset = offset;
        m_MappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(Context::Device->GetHandle(), 1, &m_MappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo Buffer::GetDescriptorInfo(const VkDeviceSize size, const VkDeviceSize offset) const
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
    void Buffer::WriteToIndex(const void* data, const int index) const
    {
        WriteToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
    }

    /**
     *  Flush the memory range at index * m_AlignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult Buffer::FlushIndex(const int index) const
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
    VkDescriptorBufferInfo Buffer::GetDescriptorInfoForIndex(const int index) const
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
    VkResult Buffer::InvalidateIndex(const int index) const
    {
        return Invalidate(m_AlignmentSize, index * m_AlignmentSize);
    }

    void Buffer::Create(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_ASSERT(vkCreateBuffer(Context::Device->GetHandle(), &bufferInfo, nullptr, &buffer),
                  "Mixture::Vulkan::Buffer::Create() - Creation failed!")

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(Context::Device->GetHandle(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Device::FindMemoryType(memRequirements.memoryTypeBits, properties);

        VK_ASSERT(vkAllocateMemory(Context::Device->GetHandle(), &allocInfo, nullptr, &bufferMemory),
                  "Mixture::Vulkan::Buffer::Create() - Memory allocation failed!")

        vkBindBufferMemory(Context::Device->GetHandle(), buffer, bufferMemory, 0);
    }

    void Buffer::Copy(const VkBuffer srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize size, VkCommandBuffer commandBuffer)
    {
        const bool invalidCommandBuffer = commandBuffer == VK_NULL_HANDLE;
        if (invalidCommandBuffer) commandBuffer = SingleTimeCommand::Begin();
        
        VkBufferCopy copyRegion;
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        
        if (invalidCommandBuffer) SingleTimeCommand::End(commandBuffer);
    }
}
