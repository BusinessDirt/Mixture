#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{

    class DeviceMemory final
    {
    public:

        DeviceMemory(const DeviceMemory&) = delete;
        DeviceMemory& operator = (const DeviceMemory&) = delete;
        DeviceMemory& operator = (DeviceMemory&&) = delete;

        DeviceMemory(size_t size, uint32_t memoryTypeBits, VkMemoryAllocateFlags allocateFLags, VkMemoryPropertyFlags propertyFlags);
        DeviceMemory(DeviceMemory&& other) noexcept;
        ~DeviceMemory();


        void* Map(size_t offset, size_t size);
        void Unmap();

    private:
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        
    private:
        VULKAN_HANDLE(VkDeviceMemory, m_Memory);
    };

}
