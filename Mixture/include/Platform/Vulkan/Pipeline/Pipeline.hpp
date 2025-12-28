#pragma once

/**
 * @file Pipeline.hpp
 * @brief Vulkan implementation of the Pipeline interface.
 */

#include "Platform/Vulkan/Definitions.hpp"

#include "Mixture/Render/RHI/IPipeline.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Vulkan implementation of a graphics pipeline.
     */
    class Pipeline : public RHI::IPipeline
    {
    public:
        /**
         * @brief Constructs a Vulkan Pipeline.
         * 
         * @param desc The pipeline description.
         */
        Pipeline(const RHI::PipelineDesc& desc);
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
        vk::Pipeline m_Handle;
        vk::PipelineLayout m_Layout;
    };
}
