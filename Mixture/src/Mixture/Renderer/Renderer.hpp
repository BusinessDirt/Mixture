#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/RendererAPI.hpp"
#include "Mixture/Renderer/RendererSystem.hpp"

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
        
        static void PushRendererSystem(RendererSystem* system);

    private:
        static Scope<RendererAPI> s_RendererAPI;
        static std::vector<RendererSystem*> s_RendererSystems;
    };
}
    
