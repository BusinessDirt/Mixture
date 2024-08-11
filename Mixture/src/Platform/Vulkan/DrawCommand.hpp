#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/DrawCommand.hpp"

namespace Mixture::Vulkan
{
    class DrawCommand : public Mixture::DrawCommand
    {
    public:
        DrawCommand() = default;
        
        void DrawCmd(CommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount = 1,
            uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;
    };
}
