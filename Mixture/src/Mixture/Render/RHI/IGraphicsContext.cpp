#include "mxpch.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::RHI
{
    Scope<IGraphicsContext> IGraphicsContext::Create(const ApplicationDescription& appDescription)
    {
        switch (appDescription.api)
        {
            case GraphicsAPI::None:
                return nullptr;

            case GraphicsAPI::Vulkan:
                return CreateScope<Vulkan::Context>(appDescription);

            case GraphicsAPI::D3D12:
                OPAL_ERROR("Core", "D3D12 not supported yet!");
                return nullptr;

            case GraphicsAPI::OpenGL:
                OPAL_ERROR("Core", "OpenGL not supported yet!");
                return nullptr;

            case GraphicsAPI::Metal:
                OPAL_ERROR("Core", "Metal not supported yet!");
                return nullptr;
        }

        return nullptr;
    }
}
