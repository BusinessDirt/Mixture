#include "mxpch.hpp"
#include "UniformBuffer.hpp"

#include "Mixture/Renderer/Renderer.hpp"

#include "Platform/Vulkan/Buffer/UniformBuffer.hpp"

namespace Mixture
{
    Scope<UniformBuffer> UniformBuffer::Create(const UniformBufferInformation& uboInfo)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Vulkan:  return CreateScope<Vulkan::UniformBuffer>(uboInfo);
            case RendererAPI::API::OpenGL:  MX_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
            case RendererAPI::API::DirectX12:  MX_CORE_ASSERT(false, "RendererAPI::DirectX12 is currently not supported!"); return nullptr;
        }

        MX_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
