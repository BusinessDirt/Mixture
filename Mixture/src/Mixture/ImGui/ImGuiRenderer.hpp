#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

namespace Mixture
{
    class ImGuiRenderer
    {
    public:
        virtual ~ImGuiRenderer() = default;

        virtual void Init() = 0;
        virtual void BeginFrame(CommandBuffer commandBuffer) = 0;
        virtual void EndFrame(CommandBuffer commandBuffer) = 0;
        
        static Scope<ImGuiRenderer> Create();
    };
}
