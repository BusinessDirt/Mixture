#pragma once

#include "Platform/Vulkan/Base.hpp"

namespace Mixture::Vulkan
{

    class DeviceMemory final
    {
    public:

        DeviceMemory(const DeviceMemory&) = delete;
        DeviceMemory& operator=(const DeviceMemory&) = delete;

        DeviceMemory(DeviceMemory&& other) noexcept;
        DeviceMemory& operator=(DeviceMemory&& other) noexcept;

        DeviceMemory(size_t size, uint32_t memoryTypeBits, VkMemoryAllocateFlags allocateFLags, VkMemoryPropertyFlags propertyFlags);
        ~DeviceMemory();


        OPAL_NODISCARD void* Map(size_t offset, size_t size) const;
        void Unmap() const;

    private:
        static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);

    private:
        VULKAN_HANDLE(VkDeviceMemory, m_Memory);
    };

}