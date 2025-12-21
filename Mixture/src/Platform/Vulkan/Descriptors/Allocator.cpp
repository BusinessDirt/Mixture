#include "mxpch.hpp"
#include "Platform/Vulkan/Descriptors/Allocator.hpp"

namespace Mixture::Vulkan
{
    DescriptorAllocator::DescriptorAllocator(vk::Device device, uint32_t maxSets, Vector<PoolSizeRatio> poolRatios)
        : m_Device(device), m_SetsPerPool(maxSets), m_Ratios(poolRatios)
    {
        // Default ratios if none provided (Standard engine usage)
        if (m_Ratios.empty())
        {
            m_Ratios =
            {
                { vk::DescriptorType::eSampler, 0.5f },
                { vk::DescriptorType::eCombinedImageSampler, 4.0f },
                { vk::DescriptorType::eSampledImage, 4.0f },
                { vk::DescriptorType::eStorageImage, 1.0f },
                { vk::DescriptorType::eUniformTexelBuffer, 1.0f },
                { vk::DescriptorType::eStorageTexelBuffer, 1.0f },
                { vk::DescriptorType::eUniformBuffer, 2.0f },
                { vk::DescriptorType::eStorageBuffer, 2.0f },
                { vk::DescriptorType::eUniformBufferDynamic, 1.0f },
                { vk::DescriptorType::eStorageBufferDynamic, 1.0f },
                { vk::DescriptorType::eInputAttachment, 0.5f }
            };
        }
    }

    DescriptorAllocator::~DescriptorAllocator()
    {
        // Destroy all pools
        for (auto p : m_FreePools) m_Device.destroyDescriptorPool(p);
        for (auto p : m_UsedPools) m_Device.destroyDescriptorPool(p);
        if (m_CurrentPool) m_Device.destroyDescriptorPool(m_CurrentPool);
    }

    vk::DescriptorPool DescriptorAllocator::CreatePool(uint32_t count, vk::DescriptorPoolCreateFlags flags)
    {
        std::vector<vk::DescriptorPoolSize> sizes;
        sizes.reserve(m_Ratios.size());

        for (auto& ratio : m_Ratios)
            sizes.push_back({ ratio.Type, uint32_t(ratio.Ratio * count) });

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.flags = flags;
        poolInfo.maxSets = count;
        poolInfo.poolSizeCount = (uint32_t)sizes.size();
        poolInfo.pPoolSizes = sizes.data();

        return m_Device.createDescriptorPool(poolInfo);
    }

    vk::DescriptorPool DescriptorAllocator::GetPool()
    {
        // If we have a fresh pool available, use it
        if (m_FreePools.size() > 0)
        {
            vk::DescriptorPool pool = m_FreePools.back();
            m_FreePools.pop_back();
            return pool;
        }

        // Otherwise create a new one
        return CreatePool(m_SetsPerPool, vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);
    }

    bool DescriptorAllocator::Allocate(vk::DescriptorSetLayout layout, vk::DescriptorSet& outSet)
    {
        if (!m_CurrentPool) m_CurrentPool = GetPool();

        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.descriptorPool = m_CurrentPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        // Try to allocate
        vk::Result result = m_Device.allocateDescriptorSets(&allocInfo, &outSet);
        if (result == vk::Result::eSuccess) return true;

        // If failed (OOM or Pool Fragmented), move current pool to "Used" and get a new one
        if (result == vk::Result::eErrorFragmentedPool || result == vk::Result::eErrorOutOfPoolMemory)
        {
            m_UsedPools.push_back(m_CurrentPool);
            m_CurrentPool = GetPool();

            // Retry allocation with new pool
            allocInfo.descriptorPool = m_CurrentPool;
            result = m_Device.allocateDescriptorSets(&allocInfo, &outSet);

            return (result == vk::Result::eSuccess);
        }

        return false; // Fatal error
    }

    void DescriptorAllocator::ResetPools()
    {
        // Reset every pool and move them to "Free"
        for (auto p : m_UsedPools)
        {
            m_Device.resetDescriptorPool(p);
            m_FreePools.push_back(p);
        }

        m_UsedPools.clear();

        if (m_CurrentPool)
        {
            m_Device.resetDescriptorPool(m_CurrentPool);
            m_FreePools.push_back(m_CurrentPool);
            m_CurrentPool = nullptr;
        }
    }
}
