#include "mxpch.hpp"
#include "Platform/Vulkan/Descriptors/Builder.hpp"

namespace Mixture::Vulkan
{
    DescriptorBuilder DescriptorBuilder::Begin(DescriptorAllocator* allocator, DescriptorLayoutCache* cache)
    {
        return DescriptorBuilder(allocator, cache);
    }

    DescriptorBuilder& DescriptorBuilder::BindBuffer(uint32_t binding, vk::DescriptorBufferInfo* bufferInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
    {
        vk::WriteDescriptorSet write;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = bufferInfo;
        m_Writes.push_back(write);

        vk::DescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = type;
        layoutBinding.stageFlags = stageFlags;
        m_Bindings.push_back(layoutBinding);

        return *this;
    }

    DescriptorBuilder& DescriptorBuilder::BindImage(uint32_t binding, vk::DescriptorImageInfo* imageInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
    {
        vk::WriteDescriptorSet write;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = imageInfo;
        m_Writes.push_back(write);

        vk::DescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = type;
        layoutBinding.stageFlags = stageFlags;
        m_Bindings.push_back(layoutBinding);

        return *this;
    }

    bool DescriptorBuilder::Build(vk::DescriptorSet& outSet, vk::DescriptorSetLayout& outLayout)
    {
        // Create Layout Info
        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.bindingCount = (uint32_t)m_Bindings.size();
        layoutInfo.pBindings = m_Bindings.data();

        // Ask Cache for Layout
        outLayout = m_LayoutCache->CreateDescriptorLayout(&layoutInfo);

        // Allocate Set (using the layout we just got)
        bool success = m_Allocator->Allocate(outLayout, outSet);
        if (!success) return false;

        for (auto& w : m_Writes) w.dstSet = outSet;
        m_Allocator->GetDevice().updateDescriptorSets(m_Writes, {});

        return true;
    }
}
