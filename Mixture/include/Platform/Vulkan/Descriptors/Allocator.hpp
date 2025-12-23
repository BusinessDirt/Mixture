#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    struct PoolSizeRatio
    {
        vk::DescriptorType Type;
        float Ratio; // e.g., 2.0 means allocate 2x as many of this type as sets
    };

    class DescriptorAllocator
    {
    public:
        DescriptorAllocator(Device& device, uint32_t maxSets, Vector<PoolSizeRatio> poolRatios);
        ~DescriptorAllocator();

        // The main function you call
        bool Allocate(vk::DescriptorSetLayout layout, vk::DescriptorSet& outSet);

        // Call this at the start of the frame to wipe all sets (if using frame-based allocation)
        void ResetPools();

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

    class DescriptorAllocators
    {
    public:
        DescriptorAllocators(Device& device, uint32_t count, uint32_t maxSets = 1000, Vector<PoolSizeRatio> poolRatios = {});
        ~DescriptorAllocators();

        DescriptorAllocator* Get(uint32_t index) const { return m_Allocators[index].get(); }

    private:
        Vector<Scope<DescriptorAllocator>> m_Allocators;
    };
}
