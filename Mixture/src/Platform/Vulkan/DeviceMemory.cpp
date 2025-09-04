#include "mxpch.hpp"
#include "Platform/Vulkan/DeviceMemory.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan 
{

    DeviceMemory::DeviceMemory(const size_t size, const uint32_t memoryTypeBits, const VkMemoryAllocateFlags allocateFLags, const VkMemoryPropertyFlags propertyFlags)
    {
        VkMemoryAllocateFlagsInfo flagsInfo = {};
        flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        flagsInfo.pNext = nullptr;
        flagsInfo.flags = allocateFLags;

        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = &flagsInfo;
        allocInfo.allocationSize = size;
        allocInfo.memoryTypeIndex = Device::FindMemoryType(memoryTypeBits, propertyFlags);

        VK_ASSERT(vkAllocateMemory(Context::Device->GetHandle(), &allocInfo, nullptr, &m_Memory),
                  "Mixture::Vulkan::DeviceMemory::DeviceMemory() - Allocation failed!")
    }

    DeviceMemory::DeviceMemory(DeviceMemory&& other) noexcept
        : m_Memory(other.m_Memory)
    {
        other.m_Memory = nullptr;
    }

    DeviceMemory& DeviceMemory::operator=(DeviceMemory&& other) noexcept
    {
        if (this != &other)
        {
            if (m_Memory)
            {
                vkFreeMemory(Context::Device->GetHandle(), m_Memory, nullptr);
                m_Memory = nullptr;
            }
            
            m_Memory = other.m_Memory;
            other.m_Memory = VK_NULL_HANDLE;
        }
        return *this;
    }

    DeviceMemory::~DeviceMemory()
    {
        if (m_Memory)
        {
            vkFreeMemory(Context::Device->GetHandle(), m_Memory, nullptr);
            m_Memory = nullptr;
        }
    }

    void* DeviceMemory::Map(const size_t offset, const size_t size) const
    {
        void* data;
        VK_ASSERT(vkMapMemory(Context::Device->GetHandle(), m_Memory, offset, size, 0, &data),
                  "Mixture::Vulkan::DeviceMemory::Map() - Failed!")

        return data;
    }

    void DeviceMemory::Unmap() const
    {
        vkUnmapMemory(Context::Device->GetHandle(), m_Memory);
    }
}
