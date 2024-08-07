#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/RendererAPI.hpp"

namespace Mixture
{
    class Renderer
    {
    public:
        static void Init(const std::string& applicationName);
		static void Shutdown();
		
		static void OnWindowResize(uint32_t width, uint32_t height);

        static void DrawFrame();

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

    private:
        static Scope<RendererAPI> s_RendererAPI;
    };
}
    