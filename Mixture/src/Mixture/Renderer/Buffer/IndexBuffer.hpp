#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

namespace Mixture
{
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;
        
        virtual void Bind(CommandBuffer commandBuffer) = 0;
        virtual uint32_t GetIndexCount() = 0;
        
        static Scope<IndexBuffer> Create(const std::vector<uint32_t>& indices);
    };
}
