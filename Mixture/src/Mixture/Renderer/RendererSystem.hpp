#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/Pipeline/GraphicsPipeline.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"
#include "Mixture/Renderer/Buffer/VertexBuffer.hpp"
#include "Mixture/Renderer/Buffer/IndexBuffer.hpp"
#include "Mixture/Renderer/Buffer/UniformBuffer.hpp"

namespace Mixture
{
    class RendererSystem
    {
    public:
        MX_NON_COPIABLE(RendererSystem);
        
        RendererSystem();
        ~RendererSystem();
        
        void Update(const FrameInfo& frameInfo);
        void Draw(const FrameInfo& frameInfo);
        
    private:
        Scope<GraphicsPipeline> m_Pipeline;
        Scope<VertexBuffer> m_VertexBuffer;
        Scope<IndexBuffer> m_IndexBuffer;
        Scope<UniformBuffer> m_UniformBuffer;
    };
}
