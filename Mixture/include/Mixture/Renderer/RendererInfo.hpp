#pragma once

#include <glm/glm.hpp>

namespace Mixture
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 UV;

        bool operator==(const Vertex& other) const
        {
            return Position == other.Position && Color == other.Color && UV == other.UV;
        }
    };

    struct FrameInfo
    {
        float FrameTime;
    };
}
