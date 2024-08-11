#include "mxpch.hpp"
#include "Renderer.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSets.hpp"

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
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        
        if (CommandBuffer buffer = s_RendererAPI->BeginFrame())
        {
            uint32_t frameIndex = static_cast<uint32_t>(Vulkan::Context::Get().SwapChain->GetCurrentFrameIndex());
            FrameInfo frameInfo { frameIndex, frameTime, buffer, Vulkan::Context::Get().DescriptorSetManager->GetSets().GetHandle(frameIndex) };
            
            for (auto system : s_RendererSystems)
                system->Update(frameInfo);
            
            for (auto system : s_RendererSystems)
                system->Draw(frameInfo);
            
            s_RendererAPI->EndFrame(buffer);
        }
        
        s_RendererAPI->WaitForDevice();
    }

    void Renderer::PushRendererSystem(RendererSystem* system)
    {
        s_RendererSystems.push_back(system);
    }
}
