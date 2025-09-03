#include "mxpch.hpp"
#include "Mixture/Renderer/Buffer.hpp"

#include "Platform/Vulkan/Buffer/Vertex.hpp"
#include "Platform/Vulkan/Buffer/Index.hpp"
#include "Platform/Vulkan/Buffer/Uniform.hpp"

namespace Mixture
{
    Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size, const size_t count)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    OPAL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!") return nullptr;
            case RendererAPI::API::Vulkan:  return CreateRef<Vulkan::VertexBuffer>(data, size, static_cast<uint32_t>(count));
            case RendererAPI::API::OpenGL:  OPAL_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!") return nullptr;
            case RendererAPI::API::DirectX: OPAL_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported!") return nullptr;
        }

        OPAL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(const std::vector<uint32_t>& indices)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    OPAL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!") return nullptr;
            case RendererAPI::API::Vulkan:  return CreateRef<Vulkan::IndexBuffer>(indices);
            case RendererAPI::API::OpenGL:  OPAL_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!") return nullptr;
            case RendererAPI::API::DirectX: OPAL_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported!") return nullptr;
        }

        OPAL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }

    Ref<UniformBuffer> UniformBuffer::Create(size_t instanceSize, uint32_t instanceCount)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    OPAL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!") return nullptr;
            case RendererAPI::API::Vulkan:  return CreateRef<Vulkan::UniformBuffer>(instanceSize, instanceCount);
            case RendererAPI::API::OpenGL:  OPAL_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!") return nullptr;
            case RendererAPI::API::DirectX: OPAL_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported!") return nullptr;
        }

        OPAL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }
}
