#include "mxpch.hpp"
#include "Texture.hpp"

#include "Mixture/Renderer/Renderer.hpp"

#include "Platform/Vulkan/Texture.hpp"

namespace Mixture
{
    Scope<Texture> Texture::Create(const std::string& filename, const SampledImageInformation& sampler)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Vulkan:  return CreateScope<Vulkan::Texture>(filename, sampler);
            case RendererAPI::API::OpenGL:  MX_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
            case RendererAPI::API::DirectX12:  MX_CORE_ASSERT(false, "RendererAPI::DirectX12 is currently not supported!"); return nullptr;
        }

        MX_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
