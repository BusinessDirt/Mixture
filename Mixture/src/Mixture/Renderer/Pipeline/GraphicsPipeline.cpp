#include "mxpch.hpp"
#include "GraphicsPipeline.hpp"

#include "Mixture/Renderer/Renderer.hpp"

#include "Platform/Vulkan/Pipeline/GraphicsPipeline.hpp"

namespace Mixture
{
    Scope<GraphicsPipeline> GraphicsPipeline::Create(const std::string& shaderFileName)
    {
        switch(Renderer::GetAPI())
        {
            case RendererAPI::API::None:    MX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::Vulkan:  return CreateScope<Vulkan::GraphicsPipeline>(shaderFileName);
            case RendererAPI::API::OpenGL:  MX_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
            case RendererAPI::API::DirectX12:  MX_CORE_ASSERT(false, "RendererAPI::DirectX12 is currently not supported!"); return nullptr;
        }
    }
}
