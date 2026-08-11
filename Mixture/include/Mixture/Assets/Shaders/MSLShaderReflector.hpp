#pragma once

/**
 * @file MSLShaderReflector.hpp
 * @brief MSL-specific shader bytecode reflection.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/IShaderReflector.hpp"

namespace Mixture
{
    class MSLShaderReflector : public IShaderReflector
    {
    public:
        ShaderReflectionData Reflect(const void* binaryData, size_t binarySize) const override;
    };
}
