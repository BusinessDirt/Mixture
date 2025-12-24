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
    };
}
