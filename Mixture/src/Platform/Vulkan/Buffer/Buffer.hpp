#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class Buffer
    {
    public:
        Buffer(VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void Unmap();

        void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo GetDescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void WriteToIndex(void* data, int index);
        VkResult FlushIndex(int index);
        VkDescriptorBufferInfo GetDescriptorInfoForIndex(int index);
        VkResult InvalidateIndex(int index);

        void* GetMappedMemory() const { return m_Mapped; }
        uint32_t GetInstanceCount() const { return m_InstanceCount; }
        VkDeviceSize GetInstanceSize() const { return m_InstanceSize; }
        VkDeviceSize GetAlignmentSize() const { return m_InstanceSize; }
        VkBufferUsageFlags GetUsageFlags() const { return m_UsageFlags; }
        VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_MemoryPropertyFlags; }
        VkDeviceSize GetBufferSize() const { return m_BufferSize; }
        
        static void Copy(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    private:
        static VkDeviceSize GetAlignment(VkDeviceSize m_InstanceSize, VkDeviceSize minOffsetAlignment);
        static void Create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void* m_Mapped = nullptr;
        VULKAN_HANDLE(VkBuffer, m_Buffer);
        VkDeviceMemory m_Memory = VK_NULL_HANDLE;

        VkDeviceSize m_BufferSize;
        uint32_t m_InstanceCount;
        VkDeviceSize m_InstanceSize;
        VkDeviceSize m_AlignmentSize;
        VkBufferUsageFlags m_UsageFlags;
        VkMemoryPropertyFlags m_MemoryPropertyFlags;
    };
}
