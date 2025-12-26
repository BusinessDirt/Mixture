#pragma once
#include "Platform/Vulkan/Definitions.hpp"

#include "Mixture/Render/RHI/IPipeline.hpp"

namespace Mixture::Vulkan
{
    class Pipeline : public RHI::IPipeline
    {
    public:
        Pipeline(const RHI::PipelineDesc& desc);
        ~Pipeline();

        vk::Pipeline GetHandle() const { return m_Handle; }
        vk::PipelineLayout GetLayout() const { return m_Layout; }

    private:
        vk::Pipeline m_Handle;
        vk::PipelineLayout m_Layout;
    };
}
