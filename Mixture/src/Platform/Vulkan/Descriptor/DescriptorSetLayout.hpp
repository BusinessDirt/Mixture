#pragma once

#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorBinding.hpp"

namespace Mixture::Vulkan
{
    class DescriptorSetLayout
    {
    public:
        MX_NON_COPIABLE(DescriptorSetLayout);

        DescriptorSetLayout(const std::vector<DescriptorBinding>& descriptorBindings);
        ~DescriptorSetLayout();

    private:
        VULKAN_HANDLE(VkDescriptorSetLayout, m_Layout);
    };
}
