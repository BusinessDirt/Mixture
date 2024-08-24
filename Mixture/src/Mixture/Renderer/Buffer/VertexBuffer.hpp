#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "Mixture/Core/Base.hpp"
#include "Mixture/Util/Util.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

namespace Mixture
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
        glm::vec3 Normal;
        glm::vec2 UV;
        
        Vertex() = default;
        
        bool operator==(const Vertex& other) const
        {
            return Position == other.Position && Color == other.Color && Normal == other.Normal && UV == other.UV;
        }
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

namespace std
{
    template<>
    struct hash<Mixture::Vertex>
    {
        size_t operator()(const Mixture::Vertex& vertex) const
        {
            size_t seed = 0;
            Mixture::HashCombine(seed, vertex.Position, vertex.Color, vertex.Normal, vertex.UV);
            return seed;
        }
    };
}
