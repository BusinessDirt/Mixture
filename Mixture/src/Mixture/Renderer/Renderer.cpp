#include "mxpch.hpp"
#include "Renderer.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{

    Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();
    std::vector<RendererSystem*> Renderer::s_RendererSystems = std::vector<RendererSystem*>();

    void Renderer::Init(const std::string& applicationName)
    {
        s_RendererAPI->Init(applicationName);
    }

    void Renderer::Shutdown()
    {
        for (auto system : s_RendererSystems)
            delete system;
        
        s_RendererAPI = nullptr;
    }
		
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        s_RendererAPI->OnWindowResize(width, height);
    }

    void Renderer::DrawFrame()
    {
        for (auto system : s_RendererSystems)
            system->Update();
        
        if (CommandBuffer buffer = s_RendererAPI->BeginFrame())
        {
            for (auto system : s_RendererSystems)
                system->Draw(buffer);
            
            s_RendererAPI->EndFrame(buffer);
        }
        
        s_RendererAPI->WaitForDevice();
    }

    void Renderer::PushRendererSystem(RendererSystem* system)
    {
        s_RendererSystems.push_back(system);
    }
}
