#include "mxpch.hpp"
#include "Mixture/Renderer/Pipeline.hpp"

#include "Mixture/Renderer/Renderer.hpp"
#include "Platform/Vulkan/GraphicsPipeline.hpp"

namespace Mixture
{
    Ref<GraphicsPipeline> GraphicsPipeline::Create(const std::string& shaderName)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    OPAL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!") return nullptr;
            case RendererAPI::API::Vulkan:  return CreateRef<Vulkan::GraphicsPipeline>(shaderName);
            case RendererAPI::API::OpenGL:  OPAL_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!") return nullptr;
            case RendererAPI::API::DirectX: OPAL_CORE_ASSERT(false, "RendererAPI::DirectX is currently not supported!") return nullptr;
        }

        OPAL_CORE_ASSERT(false, "Unknown RendererAPI!")
        return nullptr;
    }
}
