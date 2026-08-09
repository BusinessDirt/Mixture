#pragma once

/**
 * @file AllocationPolicy.hpp
 * @brief VMA allocation policies shared by Vulkan resources.
 */

#include <vma/vk_mem_alloc.h>

namespace Mixture::Vulkan::AllocationPolicy
{
    /**
     * @brief Selects device-local memory without forcing a dedicated allocation.
     *
     * vmaCreateBuffer and vmaCreateImage query Vulkan's dedicated-allocation
     * requirements and may still use dedicated memory when the driver requires
     * or recommends it. Ordinary resources remain eligible for suballocation.
     */
    inline VmaAllocationCreateInfo DeviceLocal()
    {
        VmaAllocationCreateInfo info = {};
        info.usage = VMA_MEMORY_USAGE_AUTO;
        return info;
    }

    /** @brief Selects host-visible sequential-write memory for upload buffers. */
    inline VmaAllocationCreateInfo Upload(bool persistentlyMapped = false)
    {
        VmaAllocationCreateInfo info = {};
        info.usage = VMA_MEMORY_USAGE_AUTO;
        info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        if (persistentlyMapped)
            info.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        return info;
    }
}
