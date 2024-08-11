#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

#include "Mixture/Renderer/Buffer/CommandBuffer.hpp"

namespace Mixture
{
    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0, Vulkan, OpenGL, DirectX12
        };
    public:
        virtual ~RendererAPI() = default;

        virtual void Init(const std::string& applicationName) = 0;

        virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;

        // TODO: abstract command buffer
        virtual CommandBuffer BeginFrame() = 0;
        virtual void EndFrame(CommandBuffer commandBuffer) = 0;
        virtual void WaitForDevice() = 0;

        static API GetAPI() { return s_API; }
        static Scope<RendererAPI> Create();
    private:
        static API s_API;
    };
}
    
