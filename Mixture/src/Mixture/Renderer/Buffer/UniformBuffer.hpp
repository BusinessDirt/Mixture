#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Assets/Shaders/ShaderInformation.hpp"

namespace Mixture
{
    class UniformBuffer
    {
    public:
        virtual ~UniformBuffer() = default;
        
        virtual void Update(void* data) = 0;
        
        static Scope<UniformBuffer> Create(uint32_t size);
    };
}
