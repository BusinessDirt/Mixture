#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

#define DRAW_COMMAND_STATIC(FUNCTION_NAME, ...) static void FUNCTION_NAME(__VA_ARGS__)
#define DRAW_COMMAND_VIRTUAL(FUNCTION_NAME, ...) virtual void FUNCTION_NAME##Cmd(__VA_ARGS__)

#define DRAW_COMMAND(FUNCTION_NAME, ...) public: DRAW_COMMAND_STATIC(FUNCTION_NAME, __VA_ARGS__); protected: DRAW_COMMAND_VIRTUAL(FUNCTION_NAME, __VA_ARGS__) = 0

namespace Mixture
{
    class DrawCommand
    {
    public:
        virtual ~DrawCommand() = default;
        static Scope<DrawCommand> Create();
        
        DRAW_COMMAND(Draw, CommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount = 1,
            uint32_t firstVertex = 0, uint32_t firstInstance = 0);
        
        DRAW_COMMAND(DrawIndexed, CommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0,
            int32_t vertexOffset = 0, uint32_t firstInstance = 0);

    private:
        static Scope<DrawCommand> s_DrawCommand;
    };
}
