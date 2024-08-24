#include "mxpch.hpp"
#include "DescriptorPool.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{

    DescriptorPool::DescriptorPool(const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (const DescriptorBinding& binding : descriptorBindings)
            poolSizes.push_back(VkDescriptorPoolSize{ binding.Type, static_cast<uint32_t>(binding.DescriptorCount * maxSets) });

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(maxSets);

        MX_VK_ASSERT(vkCreateDescriptorPool(Context::Get().Device->GetHandle(), &poolInfo, nullptr, &m_DescriptorPool),
            "Failed to create VkDescriptorPool");
    }

    DescriptorPool::~DescriptorPool()
    {
        if (m_DescriptorPool != nullptr)
        {
            vkDestroyDescriptorPool(Context::Get().Device->GetHandle(), m_DescriptorPool, nullptr);
            m_DescriptorPool = nullptr;
        }
    }

    Scope<DescriptorPool> DescriptorPool::CreateImGuiDescriptorPool()
    {
        DescriptorBinding imageSamplerBinding{.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
        std::vector<DescriptorBinding> bindings = { imageSamplerBinding };
        
        // 1 for ImGui itself and 1 for the viewport image
        return CreateScope<DescriptorPool>(bindings, 2);
    }
}
