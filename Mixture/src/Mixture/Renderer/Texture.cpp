#include "mxpch.hpp"
#include "Mixture/Renderer/Texture.hpp"

#include "Mixture/Renderer/Renderer.hpp"
#include "Platform/Vulkan/Texture.hpp"

namespace Mixture
{
    Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    OPAL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!") return nullptr;
            case RendererAPI::API::Vulkan:  return CreateRef<Vulkan::Texture2D>(specification);
            case RendererAPI::API::OpenGL:  OPAL_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!") return nullptr;
            case RendererAPI::API::DirectX: OPAL_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported!") return nullptr;
        }

        OPAL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }
    
    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    OPAL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!") return nullptr;
            case RendererAPI::API::Vulkan:  return CreateRef<Vulkan::Texture2D>(path);
            case RendererAPI::API::OpenGL:  OPAL_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!") return nullptr;
            case RendererAPI::API::DirectX: OPAL_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported!") return nullptr;
        }

        OPAL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }
}
