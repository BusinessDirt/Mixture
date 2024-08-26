#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/FrameInfo.hpp"

namespace Mixture
{
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;
        
        virtual void Bind(const FrameInfo& frameInfo) = 0;
        virtual void BindInstance(const FrameInfo& frameInfo) = 0;
        virtual void PushConstants(const FrameInfo& info, const void* pValues) = 0;
        
        static Scope<GraphicsPipeline> Create(const std::string& shaderFileName);
    };
}
