#include "mxpch.hpp"
#include "Renderer.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSets.hpp"

#include "Mixture/ImGui/ImGuiRenderer.hpp"

namespace Mixture
{

    Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();
    Scope<ImGuiRenderer> Renderer::s_ImGuiRenderer = ImGuiRenderer::Create();
    Scope<LayerStack> Renderer::s_LayerStack = CreateScope<LayerStack>();

    void Renderer::Init(const std::string& applicationName)
    {
        s_RendererAPI->Init(applicationName);
        s_ImGuiRenderer->Init();
    }

    void Renderer::Shutdown()
    {
        s_ImGuiRenderer = nullptr;
        s_LayerStack = nullptr;
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
            
            s_LayerStack->Update(frameInfo);
            
            s_ImGuiRenderer->BeginFrame(buffer);
            s_LayerStack->RenderUI();
            s_ImGuiRenderer->EndFrame(buffer);
            
            s_RendererAPI->EndFrame(buffer);
        }
        
        s_RendererAPI->WaitForDevice();
    }
}
