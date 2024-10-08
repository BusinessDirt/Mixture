#pragma once

#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorBinding.hpp"

namespace Mixture::Vulkan
{
    class DescriptorPool 
    {
    public:
        MX_NON_COPIABLE(DescriptorPool);

        DescriptorPool(const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
        DescriptorPool(const std::vector<VkDescriptorPoolSize> poolSizes);
        ~DescriptorPool();
        
        static Scope<DescriptorPool> CreateImGuiDescriptorPool();
        
    private:
        VULKAN_HANDLE(VkDescriptorPool, m_DescriptorPool);
    };
}
