#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Renderer/RendererAPI.hpp"
#include "Mixture/ImGui/ImGuiRenderer.hpp"
#include "Mixture/Renderer/LayerStack.hpp"

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
        
        static void PushLayer(Layer* layer) { s_LayerStack->PushLayer(layer); }
        static void PushOverlay(Layer* overlay) { s_LayerStack->PushOverlay(overlay); }

    private:
        static Scope<RendererAPI> s_RendererAPI;
        static Scope<ImGuiRenderer> s_ImGuiRenderer;
        static Scope<LayerStack> s_LayerStack;
    };
}
    
