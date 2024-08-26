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
        s_ImGuiRenderer->OnWindowResize(width, height);
    }

    void Renderer::DrawFrame(const Timestep& ts)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        startTime = currentTime;
        
        std::vector<CommandBuffer> commandBuffers{};
        
        s_ImGuiRenderer->Begin();
        s_LayerStack->RenderUI();
        s_ImGuiRenderer->End();
        
        if (CommandBuffer buffer = s_RendererAPI->BeginFrame())
        {
            uint32_t frameIndex = static_cast<uint32_t>(Vulkan::Context::Get().SwapChain->GetCurrentFrameIndex());
            FrameInfo frameInfo { 
                frameIndex, ts, buffer, 
                Vulkan::Context::Get().GlobalDescriptors->GetSets().GetHandle(frameIndex),
                Vulkan::Context::Get().InstanceDescriptors->GetSets().GetHandle(frameIndex) 
            };
            
            s_RendererAPI->BeginRenderPass(buffer);
            s_LayerStack->Update(frameInfo);
            s_RendererAPI->EndRenderPass(buffer);
            
            s_ImGuiRenderer->Render(buffer);
            
            s_RendererAPI->EndFrame(buffer);
            commandBuffers.push_back(buffer);
        }
        
        s_RendererAPI->SubmitFrame(commandBuffers);
        s_RendererAPI->WaitForDevice();
    }

    void Renderer::OnEvent(Event& e)
    {
        s_LayerStack->OnEvent(e);
    }
}
