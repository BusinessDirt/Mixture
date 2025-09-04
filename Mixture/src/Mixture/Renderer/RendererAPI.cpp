#include "mxpch.hpp"
#include "Mixture/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/RendererAPI.hpp"

namespace Mixture
{
    RendererAPI::API RendererAPI::s_API = API::Vulkan;

    Scope<RendererAPI> RendererAPI::Create()
    {
        switch (s_API)
        {
            case API::None:    OPAL_CORE_ASSERT(false, "Mixture::RendererAPI::Create() - RendererAPI::None is currently not supported!") return nullptr;
            case API::Vulkan:  return CreateScope<Vulkan::RendererAPI>();
            case API::OpenGL:  OPAL_CORE_ASSERT(false, "Mixture::RendererAPI::Create() - RendererAPI::OpenGL is currently not supported!") return nullptr;
            case API::DirectX: OPAL_CORE_ASSERT(false, "Mixture::RendererAPI::Create() - RendererAPI::DirectX is currently not supported!") return nullptr;
        }

        OPAL_CORE_ASSERT(false, "Mixture::RendererAPI::Create() - Unknown RendererAPI!")
        return nullptr;
    }
}
