#pragma once
#include "Mixture/Core/Base.hpp"

#include "Platform/Vulkan/Descriptors/LayoutCache.hpp"
#include "Platform/Vulkan/Descriptors/Allocator.hpp"
#include <vulkan/vulkan.hpp>

namespace Mixture::Vulkan
{
    class DescriptorBuilder
    {
    public:
        static DescriptorBuilder Begin(DescriptorAllocator* allocator, DescriptorLayoutCache* cache);

        DescriptorBuilder& BindBuffer(uint32_t binding, vk::DescriptorBufferInfo* bufferInfo, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlags stageFlags = vk::ShaderStageFlagBits::eAllGraphics);
        DescriptorBuilder& BindImage(uint32_t binding, vk::DescriptorImageInfo* imageInfo, vk::DescriptorType type = vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlags stageFlags = vk::ShaderStageFlagBits::eFragment);

        bool Build(vk::DescriptorSet& outSet, vk::DescriptorSetLayout& outLayout);

    private:
        DescriptorBuilder(DescriptorAllocator* allocator, DescriptorLayoutCache* cache)
            : m_Allocator(allocator), m_LayoutCache(cache)
        {}

        DescriptorAllocator* m_Allocator;
        DescriptorLayoutCache* m_LayoutCache;

        Vector<vk::DescriptorSetLayoutBinding> m_Bindings;
        Vector<vk::WriteDescriptorSet> m_Writes;
        Vector<vk::DescriptorBufferInfo*> m_BufferInfos;
        Vector<vk::DescriptorImageInfo*> m_ImageInfos;
    };
}
