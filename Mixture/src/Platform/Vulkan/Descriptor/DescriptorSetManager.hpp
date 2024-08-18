#pragma once

#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorBinding.hpp"

namespace Mixture::Vulkan
{

    class DescriptorPool;
    class DescriptorSetLayout;
    class DescriptorSets;

    class DescriptorSetManager
    {
    public:
        MX_NON_COPIABLE(DescriptorSetManager);

        explicit DescriptorSetManager(const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
        ~DescriptorSetManager();

        const DescriptorSetLayout& GetLayout() const { return *m_DescriptorSetLayout; }
        DescriptorSets& GetSets() const { return *m_DescriptorSets; }
        DescriptorPool& GetPool() const { return *m_DescriptorPool; }

    private:
        Scope<DescriptorPool> m_DescriptorPool;
        Scope<DescriptorSetLayout> m_DescriptorSetLayout;
        Scope<DescriptorSets> m_DescriptorSets;
    };
}
