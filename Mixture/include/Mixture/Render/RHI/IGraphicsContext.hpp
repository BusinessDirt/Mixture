#pragma once

#include "Mixture/Core/Base.hpp"

namespace Mixture
{
    struct ApplicationDescription;
}

namespace Mixture::RHI
{
    enum class GraphicsAPI : uint8_t
    {
        None = 0,
        Vulkan,
        D3D12,
        OpenGL,
        Metal
    };

    class IGraphicsContext
    {
    public:
        IGraphicsContext(const ApplicationDescription& appDescription) {}
        virtual ~IGraphicsContext() = default;

        virtual GraphicsAPI GetAPI() const = 0;

        static Scope<IGraphicsContext> Create(const ApplicationDescription& appDescription);
    };
}
