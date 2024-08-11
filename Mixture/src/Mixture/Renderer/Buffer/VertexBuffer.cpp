#include "mxpch.hpp"
#include "VertexBuffer.hpp"

#include "Mixture/Renderer/Renderer.hpp"

#include "Platform/Vulkan/Buffer/VertexBuffer.hpp"

namespace Mixture
{
    Scope<VertexBuffer> VertexBuffer::Create(const std::vector<Vertex>& vertices)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Vulkan:  return CreateScope<Vulkan::VertexBuffer>(vertices);
            case RendererAPI::API::OpenGL:  MX_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
            case RendererAPI::API::DirectX12:  MX_CORE_ASSERT(false, "RendererAPI::DirectX12 is currently not supported!"); return nullptr;
        }

        MX_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
