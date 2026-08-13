#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Pipeline.hpp
 * @brief Metal implementation of the Pipeline interface.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/IPipeline.hpp"

namespace Mixture::Metal
{
    class Device;

    /**
     * @brief Metal implementation of a graphics pipeline.
     */
    class Pipeline : public RHI::IPipeline
    {
    public:
        Pipeline(Ref<Device> device, const RHI::PipelineDesc& desc);
        ~Pipeline();

        bool IsValid() const override { return m_PipelineState != nullptr; }

        MTL::RenderPipelineState* GetPipelineState() const { return m_PipelineState; }
        MTL::DepthStencilState* GetDepthStencilState() const { return m_DepthStencilState; }

    private:
        Ref<Device> m_Device;
        MTL::RenderPipelineState* m_PipelineState = nullptr;
        MTL::DepthStencilState* m_DepthStencilState = nullptr;
    };
}

#endif
