#pragma once

/**
 * @file Builder.hpp
 * @brief Helper for building Vulkan Descriptor Sets.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Descriptors/Allocator.hpp"
#include "Platform/Vulkan/Descriptors/LayoutCache.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Fluent builder for creating Descriptor Sets and their Layouts.
     * 
     * Simplifies the process of creating descriptor sets by automatically handling
     * layout creation (via cache) and writing descriptor updates.
     */
    class DescriptorBuilder
    {
    public:
        /**
         * @brief Starts the building process.
         * 
         * @param allocator The allocator to use for the descriptor set.
         * @param cache The cache to use for the descriptor set layout.
         * @return DescriptorBuilder The builder instance.
         */
        static DescriptorBuilder Begin(DescriptorAllocator* allocator, DescriptorLayoutCache* cache);

        /**
         * @brief Binds a buffer to a specific binding slot.
         * 
         * @param binding The binding index in the shader.
         * @param bufferInfo Pointer to the buffer info structure.
         * @param type The type of descriptor (Uniform, Storage, etc.).
         * @param stageFlags The shader stages that access this resource.
         * @return DescriptorBuilder& Reference to the builder for chaining.
         */
        DescriptorBuilder& BindBuffer(uint32_t binding, vk::DescriptorBufferInfo* bufferInfo, vk::DescriptorType type = vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlags stageFlags = vk::ShaderStageFlagBits::eAllGraphics);

        /**
         * @brief Binds an image to a specific binding slot.
         * 
         * @param binding The binding index in the shader.
         * @param imageInfo Pointer to the image info structure.
         * @param type The type of descriptor (Sampler, CombinedImageSampler, etc.).
         * @param stageFlags The shader stages that access this resource.
         * @return DescriptorBuilder& Reference to the builder for chaining.
         */
        DescriptorBuilder& BindImage(uint32_t binding, vk::DescriptorImageInfo* imageInfo, vk::DescriptorType type = vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlags stageFlags = vk::ShaderStageFlagBits::eFragment);

        /**
         * @brief Builds the descriptor set and layout.
         * 
         * @param[out] outSet The resulting descriptor set.
         * @param[out] outLayout The resulting descriptor set layout.
         * @return true If successful.
         * @return false If failed.
         */
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
