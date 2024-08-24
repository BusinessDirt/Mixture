#pragma once

#include "Mixture/Core/Base.hpp"
#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

#include <functional>

namespace Mixture
{
    class ImGuiRenderer
    {
    public:
        virtual ~ImGuiRenderer() = default;

        virtual void Init() = 0;
        virtual void Begin() = 0;
        virtual void End() = 0;
        virtual void Render(CommandBuffer buffer) = 0;
        virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;
        virtual void* GetViewportImage() = 0;
        
        static Scope<ImGuiRenderer> Create();
    };
}
