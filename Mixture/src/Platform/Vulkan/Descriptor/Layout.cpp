#include "mxpch.hpp"
#include "Platform/Vulkan/Descriptor/Layout.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    DescriptorSetLayout::DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
        : m_Bindings(bindings)
    {
        m_Device = Context::Device->GetHandle();
        
        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = static_cast<uint32_t>(bindings.size());
        info.pBindings = bindings.data();

        VK_ASSERT(vkCreateDescriptorSetLayout(m_Device, &info, nullptr, &m_Layout),
                  "Failed to create DescriptorSetLayout")
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        if (m_Layout) vkDestroyDescriptorSetLayout(m_Device, m_Layout, nullptr);
    }
}
