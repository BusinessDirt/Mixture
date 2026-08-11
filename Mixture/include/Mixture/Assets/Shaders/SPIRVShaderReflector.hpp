#pragma once

/**
 * @file SPIRVShaderReflector.hpp
 * @brief SPIRV-specific shader bytecode reflection.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/IShaderReflector.hpp"

namespace Mixture
{
    class SPIRVShaderReflector : public IShaderReflector
    {
    public:
        ShaderReflectionData Reflect(const void* binaryData, size_t binarySize) const override;
    };
}
