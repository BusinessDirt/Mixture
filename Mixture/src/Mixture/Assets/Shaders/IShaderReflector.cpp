#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/IShaderReflector.hpp"

#include "Mixture/Assets/Shaders/SPIRVShaderReflector.hpp"
#include "Mixture/Assets/Shaders/DXILShaderReflector.hpp"
#include "Mixture/Assets/Shaders/MSLShaderReflector.hpp"

namespace Mixture
{
    Scope<IShaderReflector> IShaderReflector::Create(RHI::GraphicsAPI graphicsAPI)
    {
        switch (graphicsAPI)
        {
            case RHI::GraphicsAPI::Vulkan:
                // DXC currently emits SPIR-V for Vulkan.
                return CreateScope<SPIRVShaderReflector>();
            case RHI::GraphicsAPI::Metal:
                // The current compiler path also emits SPIR-V for Metal. Slang can
                // replace this with a Metal-specific reflector when it emits Metal bytecode.
                return CreateScope<MSLShaderReflector>();
            case RHI::GraphicsAPI::D3D12:
                return CreateScope<DXILShaderReflector>();
            case RHI::GraphicsAPI::None:
                return nullptr;
        }

        return nullptr;
    }
}
