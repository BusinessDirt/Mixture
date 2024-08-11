#include "mxpch.hpp"
#include "DescriptorSetManager.hpp"

#include "Platform/Vulkan/Descriptor/DescriptorPool.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSetLayout.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSets.hpp"

namespace Mixture::Vulkan
{
    DescriptorSetManager::DescriptorSetManager(const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets)
    {
        std::map<uint32_t, VkDescriptorType> bindingTypes;

        for (const DescriptorBinding& binding : descriptorBindings)
        {
            if (!bindingTypes.insert(std::make_pair(binding.Binding, binding.Type)).second)
            {
                MX_CORE_ERROR("Binding collision");
            }
        }

        m_DescriptorPool.reset(new DescriptorPool(descriptorBindings, maxSets));
        m_DescriptorSetLayout.reset(new DescriptorSetLayout(descriptorBindings));
        m_DescriptorSets.reset(new DescriptorSets(*m_DescriptorPool, *m_DescriptorSetLayout, bindingTypes, maxSets));
    }

    DescriptorSetManager::~DescriptorSetManager()
    {
        m_DescriptorSets = nullptr;
        m_DescriptorSetLayout = nullptr;
        m_DescriptorPool = nullptr;
    }
}
