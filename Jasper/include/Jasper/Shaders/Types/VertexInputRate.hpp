#pragma once

#include <cstdint>

namespace Jasper
{
    enum VertexInputRate : uint8_t
    {
        PerVertex = 0,
        PerInstance = 1
    };

    namespace Util
    {
        const char* VertexInputRateToString(VertexInputRate vertexInputRate);
    }
}
