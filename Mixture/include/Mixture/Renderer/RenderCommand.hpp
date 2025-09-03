#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/Buffer.hpp"

namespace Mixture
{
    class RenderCommand
    {
    public:
        static void Draw(const Ref<VertexBuffer>& vertexBuffer)
        {
            Renderer::s_RendererAPI->Draw(vertexBuffer->GetCount(), 1, 0, 0);
        }
        
        static void DrawIndexed(const Ref<IndexBuffer>& indexBuffer)
        {
            Renderer::s_RendererAPI->DrawIndexed(indexBuffer->GetCount(), 1, 0, 0, 0);
        }
    };
}
