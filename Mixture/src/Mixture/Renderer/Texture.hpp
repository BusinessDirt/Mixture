#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/ShaderInformation.hpp"

namespace Mixture
{
    class Texture
    {
    public:
        virtual ~Texture() = default;
        
        static Scope<Texture> Create(const std::string& filename);
    };
}
