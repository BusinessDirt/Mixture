#pragma once

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"

namespace Mixture
{
    class RenderGraph;

    struct ImGuiPassData
    {
        RGResourceHandle Output;
    };

    class ImGuiPass
    {
    public:
        static void AddToGraph(RenderGraph& graph, RGResourceHandle outputHandle);
    };
}
