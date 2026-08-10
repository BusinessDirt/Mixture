#include "mxpch.hpp"
#include "Platform/Vulkan/Descriptors/Builder.hpp"

namespace Mixture::Vulkan
{
    DescriptorBuilder DescriptorBuilder::Begin(DescriptorAllocator* allocator, DescriptorLayoutCache* cache)
    {
        return DescriptorBuilder(allocator, cache);
    }

    DescriptorBuilder& DescriptorBuilder::BindBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
    {
        vk::WriteDescriptorSet write;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = nullptr;
        m_Writes.push_back(write);
        m_BufferInfos.push_back(bufferInfo);

        vk::DescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = type;
        layoutBinding.stageFlags = stageFlags;
        m_Bindings.push_back(layoutBinding);

        return *this;
    }

    DescriptorBuilder& DescriptorBuilder::BindImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
    {
        vk::WriteDescriptorSet write;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = nullptr;
        m_Writes.push_back(write);
        m_ImageInfos.push_back(imageInfo);

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

        return BuildWithLayout(outSet, outLayout);
    }

    bool DescriptorBuilder::BuildWithLayout(vk::DescriptorSet& outSet, vk::DescriptorSetLayout layout)
    {
        bool success = m_Allocator->Allocate(layout, outSet);
        if (!success) return false;

        size_t bufferIndex = 0;
        size_t imageIndex = 0;
        for (auto& write : m_Writes)
        {
            write.dstSet = outSet;
            if (write.descriptorType == vk::DescriptorType::eUniformBuffer
                || write.descriptorType == vk::DescriptorType::eStorageBuffer)
                write.pBufferInfo = &m_BufferInfos[bufferIndex++];
            else
                write.pImageInfo = &m_ImageInfos[imageIndex++];
        }
        m_Allocator->GetDevice()->GetHandle().updateDescriptorSets(m_Writes, {});

        return true;
    }
}
