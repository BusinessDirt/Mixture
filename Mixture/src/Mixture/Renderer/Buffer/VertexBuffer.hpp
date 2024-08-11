#pragma once

#include <glm/glm.hpp>

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

namespace Mixture
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;
    };

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;
        
        virtual void Bind(CommandBuffer commandBuffer) = 0;
        virtual uint32_t GetVertexCount() = 0;
        
        static Scope<VertexBuffer> Create(const std::vector<Vertex>& vertices);
    };
}
