#pragma once
#include "Mixture/Core/Base.hpp"

namespace Mixture
{
    class ShaderCompiler
    {
    public:
        static Vector<uint8_t> Compile(const std::string& source);
        static Vector<uint8_t> ConvertToMSL(const Vector<uint8_t>& spv);
    };
}
