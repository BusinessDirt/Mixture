#pragma once

#include <glm/vec4.hpp>

#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/Context.hpp"

namespace Mixture
{
    class RendererAPI
    {
    public:
        enum class API : uint8_t
        {
            None = 0, Vulkan = 1, OpenGL = 2, DirectX = 3
        };
        
    public:
        virtual ~RendererAPI() = default;

        virtual void Initialize(const std::string& applicationName) = 0;
        virtual void DestroyImGuiContext() = 0;
        virtual void Shutdown() = 0;
        
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
		
        virtual void BeginSceneRenderpass() = 0;
        virtual void EndSceneRenderpass() = 0;
		
        virtual void BeginImGuiImpl() = 0;
        virtual void RenderImGui() = 0;

        virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

        virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;

        static API GetAPI() { return s_API; }
        static Scope<RendererAPI> Create();

    private:
        static API s_API;
    };
}
