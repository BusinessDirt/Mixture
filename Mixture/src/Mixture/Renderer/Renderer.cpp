#include "mxpch.h"
#include "Renderer.h"

namespace Mixture
{

    Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();

    void Renderer::Init(const std::string& applicationName)
    {
        s_RendererAPI->Init(applicationName);
    }

    void Renderer::Shutdown()
    {
        s_RendererAPI = nullptr;
    }
		
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        s_RendererAPI->OnWindowResize(width, height);
    }

    void Renderer::DrawFrame()
    {
        if (s_RendererAPI->BeginFrame())
        {
            s_RendererAPI->EndFrame();
        }
    }
}