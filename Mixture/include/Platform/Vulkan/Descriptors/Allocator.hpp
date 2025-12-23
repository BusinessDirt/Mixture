#pragma once

/**
 * @file Allocator.hpp
 * @brief Vulkan Descriptor Allocator implementation.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Defines the ratio of descriptor types to allocate in a pool.
     */
    struct PoolSizeRatio
    {
        vk::DescriptorType Type;
        float Ratio; // e.g., 2.0 means allocate 2x as many of this type as sets
    };

    /**
     * @brief Manages a growing pool of descriptors.
     * 
     * Handles allocation of descriptor sets, automatically creating new pools as needed.
     */
    class DescriptorAllocator
    {
    public:
        /**
         * @brief Constructor.
         * 
         * @param device Reference to the logical device.
         * @param maxSets Initial maximum number of sets per pool.
         * @param poolRatios Ratios of descriptor types to allocate.
         */
        DescriptorAllocator(Device& device, uint32_t maxSets, Vector<PoolSizeRatio> poolRatios);
        ~DescriptorAllocator();

        /**
         * @brief Allocates a descriptor set from the current pool.
         * 
         * @param layout The descriptor set layout to allocate.
         * @param[out] outSet The allocated descriptor set.
         * @return true If allocation succeeded.
         * @return false If allocation failed.
         */
        bool Allocate(vk::DescriptorSetLayout layout, vk::DescriptorSet& outSet);

        /**
         * @brief Resets all pools, making all allocated sets invalid.
         * 
         * Useful for frame-based allocation where all sets are discarded at the end of a frame.
         */
        void ResetPools();

        /**
         * @brief Gets the associated device.
         * 
         * @return Device* Pointer to the device.
         */
        Device* GetDevice() { return m_Device; }

    private:
        vk::DescriptorPool GetPool();
        vk::DescriptorPool CreatePool(uint32_t count, vk::DescriptorPoolCreateFlags flags);

    private:
        Device* m_Device;

        vk::DescriptorPool m_CurrentPool = nullptr; // The active pool
        Vector<vk::DescriptorPool> m_UsedPools; // Full pools
        Vector<vk::DescriptorPool> m_FreePools; // Reset/Empty pools ready for reuse

        Vector<PoolSizeRatio> m_Ratios;
        uint32_t m_SetsPerPool;
    };

    /**
     * @brief Manages multiple DescriptorAllocators, typically one per frame in flight.
     */
    class DescriptorAllocators
    {
    public:
        /**
         * @brief Constructor.
         * 
         * @param device Reference to the logical device.
         * @param count Number of allocators to create (usually matches frames in flight).
         * @param maxSets Maximum sets per allocator pool.
         * @param poolRatios Ratios of descriptor types.
         */
        DescriptorAllocators(Device& device, uint32_t count, uint32_t maxSets = 1000, Vector<PoolSizeRatio> poolRatios = {});
        ~DescriptorAllocators();

        /**
         * @brief Gets the allocator at the specified index.
         * 
         * @param index Index of the allocator.
         * @return DescriptorAllocator* Pointer to the allocator.
         */
        DescriptorAllocator* Get(uint32_t index) const { return m_Allocators[index].get(); }

    private:
        Vector<Scope<DescriptorAllocator>> m_Allocators;
    };
}
