#include "mxpch.hpp"
#include "Platform/Vulkan/Descriptor/Pool.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    DescriptorPool::DescriptorPool()
    {
        m_Device = Context::Device->GetHandle();
        m_FramesInFlight = Swapchain::MAX_FRAMES_IN_FLIGHT;
        
        // ---- Create Global Pool ----
        const std::vector<VkDescriptorPoolSize> globalPoolSizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 100 },
            {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 500 }
        };
        
        m_GlobalPool = CreatePool(600, globalPoolSizes, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
        
        // --- Create Per-Frame Pools ---
        const std::vector<VkDescriptorPoolSize> framePoolSizes = {
            {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = 200 }
        };

        m_FramePools.resize(m_FramesInFlight);
        for (uint32_t i = 0; i < m_FramesInFlight; i++)
        {
            m_FramePools[i] = CreatePool(200, framePoolSizes, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
        }
    }

    DescriptorPool::~DescriptorPool()
    {
        for (const auto pool : m_FramePools) vkDestroyDescriptorPool(m_Device, pool, nullptr);
        vkDestroyDescriptorPool(m_Device, m_GlobalPool, nullptr);
    }

    VkDescriptorPool DescriptorPool::CreatePool(const uint32_t maxSets, const std::vector<VkDescriptorPoolSize> &poolSizes, const VkDescriptorPoolCreateFlags flags) const
    {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = maxSets;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.flags = flags;

        VkDescriptorPool pool;
        VK_ASSERT(vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &pool),
                  "Mixture::Vulkan::DescriptorPool::CreatePool() - Creation failed!")
        
        return pool;
    }

    DescriptorSet DescriptorPool::AllocateGlobalSet(const Ref<DescriptorSetLayout>& layout) const
    {
        return { m_GlobalPool, layout };
    }

    DescriptorSet DescriptorPool::AllocateFrameSet(const Ref<DescriptorSetLayout>& layout, const uint32_t frameIndex) const
    {
        return { m_FramePools[frameIndex], layout };
    }

    void DescriptorPool::ResetFramePool(const uint32_t frameIndex) const
    {
        vkResetDescriptorPool(m_Device, m_FramePools[frameIndex], 0);
    }

    void DescriptorPool::FreeGlobalSet(const DescriptorSet& set) const
    {
        set.Free(m_GlobalPool);
    }
}
