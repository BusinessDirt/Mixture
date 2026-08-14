#pragma once

/**
 * @file Pipeline.hpp
 * @brief Vulkan implementation of the Pipeline interface.
 */

#include "Platform/Vulkan/Definitions.hpp"

#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Assets/Shaders/SlangShaderReflector.hpp"

namespace Mixture::Vulkan
{
    struct PipelineLayoutDescription
    {
        Vector<Vector<vk::DescriptorSetLayoutBinding>> Sets;
        Vector<vk::PushConstantRange> PushConstants;
    };

    PipelineLayoutDescription BuildPipelineLayoutDescription(
        const Vector<std::pair<const ShaderReflectionData*, RHI::ShaderStage>>& shaders);

    class Device;

    /**
     * @brief Vulkan implementation of a graphics pipeline.
     */
    class Pipeline : public RHI::IPipeline
    {
    public:
        /**
         * @brief Constructs a Vulkan Pipeline.
         * 
         * @param device Shared ownership of the creating device.
         * @param desc The pipeline description.
         */
        Pipeline(Ref<Device> device, const RHI::PipelineDesc& desc);
        ~Pipeline();

        bool IsValid() const override { return static_cast<bool>(m_Handle) && static_cast<bool>(m_Layout); }

        const ShaderReflectionData* GetShaderReflectionData() const override { return m_ReflectionData; }

        /**
         * @brief Gets the Vulkan Pipeline handle.
         * 
         * @return vk::Pipeline The raw handle.
         */
        vk::Pipeline GetHandle() const { return m_Handle; }

        /**
         * @brief Gets the Vulkan Pipeline Layout.
         * 
         * @return vk::PipelineLayout The layout handle.
         */
        vk::PipelineLayout GetLayout() const { return m_Layout; }
        vk::DescriptorSetLayout GetDescriptorSetLayout(uint32_t set) const
        {
            return set < m_DescriptorSetLayouts.size() ? m_DescriptorSetLayouts[set] : vk::DescriptorSetLayout{};
        }
        const vk::PushConstantRange* FindPushConstantRange(vk::ShaderStageFlags stage, uint32_t size) const;

    private:
        Ref<Device> m_Device;
        vk::Pipeline m_Handle = nullptr;
        vk::PipelineLayout m_Layout = nullptr;
        Vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts;
        Vector<vk::PushConstantRange> m_PushConstantRanges;
        const ShaderReflectionData* m_ReflectionData = nullptr;
    };
}
