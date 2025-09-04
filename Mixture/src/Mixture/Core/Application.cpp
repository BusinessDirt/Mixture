#include "mxpch.hpp"
#include "Mixture/Core/Application.hpp"

#include "Mixture/Renderer/Renderer.hpp"
#include "Mixture/Core/Time.hpp"

#include <Opal/Base.hpp>
#include <ranges>

namespace Mixture
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name, ApplicationCommandLineArgs args)
    {
        OPAL_CORE_ASSERT(!s_Instance, "Mixture::Application::Application() - Application already exists!")
        
        s_Instance = this;

        auto props = WindowProps();
        props.Title = name;
        
        m_Window = CreateScope<Window>(props);
        m_Window->SetEventCallback(OPAL_BIND_EVENT_FN(OnEvent));

        m_AssetManager = CreateScope<AssetManager>();
        
        PushOverlay(new ImGuiLayer());
        Renderer::Initialize(name);
    }

    Application::~Application()
    {
        Renderer::DestroyImGuiContext();
        m_LayerStack.Shutdown(); 
        Renderer::Shutdown();
        
        m_AssetManager.reset();
        m_Window.reset();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::Run() const
    {
        Timer frameTimer{};
        FrameInfo frameInfo{};
        
        while (m_Running)
        {
            m_Window->OnUpdate();
            
            // Update information about current frame
            frameInfo.FrameTime = frameTimer.Tick();
            
            // Update Layers
            for (Layer* layer : m_LayerStack) layer->OnUpdate(frameInfo);

            // Draw to ImGui
            Renderer::BeginImGuiImpl();
            ImGuiLayer::Begin();
            for (Layer* layer : m_LayerStack) layer->OnRenderImGui(frameInfo);
            ImGuiLayer::End();
            
            Renderer::BeginFrame();
            {
                // Draw Scene
                Renderer::BeginSceneRenderpass();
                for (Layer* layer : m_LayerStack) layer->OnRender(frameInfo);
                Renderer::EndSceneRenderpass();

                // Draw UI
                Renderer::RenderImGui();
            }
            Renderer::EndFrame();
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(OPAL_BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<FramebufferResizeEvent>(OPAL_BIND_EVENT_FN(OnFramebufferResize));

        for (const auto& it : std::ranges::reverse_view(m_LayerStack))
        {
            if (event.Handled) break;
            it->OnEvent(event);
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnFramebufferResize(const FramebufferResizeEvent& e)
    {
        Renderer::OnFramebufferResize(e.GetWidth(), e.GetHeight());
        return false;
    }
}
