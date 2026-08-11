#pragma once

/**
 * @file DXILShaderReflector.hpp
 * @brief DXIL-specific shader bytecode reflection.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/IShaderReflector.hpp"

namespace Mixture
{
    class DXILShaderReflector : public IShaderReflector
    {
    public:
        ShaderReflectionData Reflect(const void* binaryData, size_t binarySize) const override;
    };
}
