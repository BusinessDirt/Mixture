#pragma once

/**
 * @file Pipeline.hpp
 * @brief Vulkan implementation of the Pipeline interface.
 */

#include "Platform/Vulkan/Definitions.hpp"

#include "Mixture/Render/RHI/IPipeline.hpp"

namespace Mixture::Vulkan
{
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

    private:
        Ref<Device> m_Device;
        vk::Pipeline m_Handle = nullptr;
        vk::PipelineLayout m_Layout = nullptr;
    };
}
