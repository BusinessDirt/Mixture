#include "mxpch.hpp"
#include "Mixture/Renderer/Renderer.hpp"

namespace Mixture
{
    Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();
    
    void Renderer::Initialize(const std::string& applicationName)
    {
        s_RendererAPI->Initialize(applicationName);
    }

    void Renderer::DestroyImGuiContext()
    {
        s_RendererAPI->DestroyImGuiContext();
    }

    void Renderer::Shutdown()
    {
        s_RendererAPI->Shutdown();
    }

    void Renderer::BeginFrame()
    {
        s_RendererAPI->BeginFrame();
    }

    void Renderer::EndFrame()
    {
        s_RendererAPI->EndFrame();
    }

    void Renderer::BeginSceneRenderpass()
    {
        s_RendererAPI->BeginSceneRenderpass();
    }

    void Renderer::EndSceneRenderpass()
    {
        s_RendererAPI->EndSceneRenderpass();
    }

    void Renderer::BeginImGuiImpl()
    {
        s_RendererAPI->BeginImGuiImpl();
    }

    void Renderer::RenderImGui()
    {
        s_RendererAPI->RenderImGui();
    }
}
