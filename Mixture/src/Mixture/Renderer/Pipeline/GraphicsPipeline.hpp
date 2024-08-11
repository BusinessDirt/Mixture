#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

namespace Mixture
{
    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;
        
        virtual void Bind(CommandBuffer commandBuffer) = 0;
        
        static Scope<GraphicsPipeline> Create(const std::string& shaderFileName);
    };
}
