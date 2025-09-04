#include "Jasper/Shaders/Types/VertexInputRate.hpp"

namespace Jasper::Util
{
    const char* VertexInputRateToString(const VertexInputRate vertexInputRate)
    {
        switch (vertexInputRate)
        {
            case PerVertex:   return "Per Vertex";
            case PerInstance: return "Per Instance";
        }

        return "Unknown VertexInputRate";
    }
}
