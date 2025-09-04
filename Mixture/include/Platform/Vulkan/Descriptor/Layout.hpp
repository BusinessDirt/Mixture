#pragma once

#include "Jasper.hpp"

#include "Platform/Vulkan/Base.hpp"

namespace Mixture::Vulkan
{
    class DescriptorSetLayout
    {
    public:
        OPAL_NON_COPIABLE(DescriptorSetLayout);
        
        explicit DescriptorSetLayout(const std::vector<Jasper::DescriptorSetLayoutBinding>& bindings);
        ~DescriptorSetLayout();

        OPAL_NODISCARD const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const { return m_Bindings; }

    private:
        VULKAN_HANDLE(VkDescriptorSetLayout, m_Layout);
        VkDevice m_Device;
        std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
    };
}
