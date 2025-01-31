#include "mxpch.hpp"
#include "DescriptorSetLayout.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    DescriptorSetLayout::DescriptorSetLayout(const std::vector<DescriptorBinding>& descriptorBindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

        for (const DescriptorBinding& binding : descriptorBindings)
        {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = binding.Binding;
            layoutBinding.descriptorCount = binding.DescriptorCount;
            layoutBinding.descriptorType = binding.Type;
            layoutBinding.stageFlags = binding.Stage;

            layoutBindings.push_back(layoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        layoutInfo.pBindings = layoutBindings.data();

        MX_VK_ASSERT(vkCreateDescriptorSetLayout(Context::Get().GetDevice().GetHandle(), &layoutInfo, nullptr, &m_Layout),
            "Failed to create VkDescriptorSetLayout");
    }

    DescriptorSetLayout::~DescriptorSetLayout()
    {
        if (m_Layout != nullptr)
        {
            vkDestroyDescriptorSetLayout(Context::Get().GetDevice().GetHandle(), m_Layout, nullptr);
            m_Layout = nullptr;
        }
    }
}
