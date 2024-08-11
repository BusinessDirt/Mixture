#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/Pipeline/GraphicsPipeline.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"
#include "Mixture/Renderer/Buffer/VertexBuffer.hpp"

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
        Scope<VertexBuffer> m_VertexBuffer;
    };
}
