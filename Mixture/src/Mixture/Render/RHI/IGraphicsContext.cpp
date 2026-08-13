#include "mxpch.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::RHI
{
    Scope<IGraphicsContext> IGraphicsContext::Create(const ApplicationDescription& appDescription, void* windowHandle)
    {
        switch (appDescription.API)
        {
            case GraphicsAPI::None:
                return nullptr;

            case GraphicsAPI::Vulkan:
                return CreateScope<Vulkan::Context>(appDescription, windowHandle);

#ifdef OPAL_PLATFORM_WINDOWS
            case GraphicsAPI::D3D12:
                OPAL_ERROR("Core", "D3D12 is not supported yet!");
                return nullptr;
#else
            case GraphicsAPI::D3D12:
                OPAL_ERROR("Core", "D3D12 is not supported on this platform!");
                return nullptr;
#endif

#ifdef OPAL_PLATFORM_DARWIN
            case GraphicsAPI::Metal:
                OPAL_ERROR("Core", "Metal is not supported yet!");
                return nullptr;
#else
            case GraphicsAPI::Metal:
                OPAL_ERROR("Core", "Metal is not supported on this platform!");
                return nullptr;
#endif
        }

        return nullptr;
    }
}
