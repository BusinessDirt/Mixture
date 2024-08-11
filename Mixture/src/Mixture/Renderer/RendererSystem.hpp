#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/Pipeline/GraphicsPipeline.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

namespace Mixture
{
    class RendererSystem
    {
    public:
        MX_NON_COPIABLE(RendererSystem);
        
        RendererSystem();
        ~RendererSystem();
        
        void Update();
        void Draw(CommandBuffer commandBuffer);
        
    private:
        Scope<GraphicsPipeline> m_Pipeline;
    };
}
