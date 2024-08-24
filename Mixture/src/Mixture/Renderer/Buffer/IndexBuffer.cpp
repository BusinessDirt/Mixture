#include "mxpch.hpp"
#include "IndexBuffer.hpp"

#include "Mixture/Renderer/Renderer.hpp"

#include "Platform/Vulkan/Buffer/IndexBuffer.hpp"

namespace Mixture
{
    Scope<IndexBuffer> IndexBuffer::Create(const std::vector<uint32_t>& indices)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Vulkan:  return CreateScope<Vulkan::IndexBuffer>(indices);
            case RendererAPI::API::OpenGL:  MX_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
            case RendererAPI::API::DirectX12:  MX_CORE_ASSERT(false, "RendererAPI::DirectX12 is currently not supported!"); return nullptr;
        }

        MX_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
