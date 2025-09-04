#include "mxpch.hpp"
#include "Platform/Vulkan/Descriptor/Layout.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    DescriptorSetLayout::DescriptorSetLayout(const std::vector<Jasper::DescriptorSetLayoutBinding>& bindings)
    {
        m_Device = Context::Device->GetHandle();
        m_Bindings.resize(bindings.size());
        for (size_t i = 0; i < bindings.size(); i++)
        {
            m_Bindings[i].binding = bindings[i].Binding;
            m_Bindings[i].stageFlags = bindings[i].StageFlags;
            m_Bindings[i].descriptorCount = bindings[i].Count;
            m_Bindings[i].descriptorType = static_cast<VkDescriptorType>(bindings[i].Type);
            m_Bindings[i].pImmutableSamplers = nullptr;
        }
        
        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = static_cast<uint32_t>(m_Bindings.size());
        info.pBindings = m_Bindings.data();

        VK_ASSERT(vkCreateDescriptorSetLayout(m_Device, &info, nullptr, &m_Layout),
                  "Mixture::Vulkan::DescriptorSetLayout::DescriptorSetLayout() - Creation failed!")
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        if (m_Layout) vkDestroyDescriptorSetLayout(m_Device, m_Layout, nullptr);
    }
}
