#include "mxpch.hpp"
#include "Mixture/Core/Application.hpp"

#include "Mixture/Core/Time.hpp"

#include <Opal/Base.hpp>
#include <ranges>

namespace Mixture
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationDescription& appDescription)
        : m_AppDescription(appDescription)
    {
        OPAL_ASSERT("Core", !s_Instance, "Mixture::Application::Application() - Application already exists!")

        s_Instance = this;

        auto props = WindowProps();
        props.Title = appDescription.Name;
        props.Width = appDescription.Width;
        props.Height = appDescription.Height;

        m_Window = CreateScope<Window>(props);
        m_Window->SetEventCallback(OPAL_BIND_EVENT_FN(OnEvent));

        m_Context = RHI::IGraphicsContext::Create(appDescription, m_Window->GetNativeWindow());
        m_RenderGraph = CreateScope<RenderGraph>();
    }

    Application::~Application()
    {
        m_LayerStack.Shutdown();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::Run() const
    {
        Timer frameTimer{};

        while (m_Running)
        {
            float timestep = frameTimer.Tick();
            m_Window->OnUpdate();

            // CPU Logic
            for (Layer* layer : m_LayerStack) layer->OnUpdate(timestep);

            // Render Setup
            m_RenderGraph->Clear();

            // IMPORT: Get the raw Swapchain image for this frame
            /*
            auto swapchainImg = m_Window->GetSwapchain()->GetCurrentImage();
            m_RenderGraph->ImportResource("Backbuffer", swapchainImg);

            // Graph Building (Layers declare their passes)
            for (Layer* layer : m_LayerStack) {
                layer->OnRender(*m_RenderGraph);
            }

            // Compile & Execute
            m_RenderGraph->Compile();

            RHI::ICommandList* cmd = m_GraphicsDevice->GetCommandList();
            cmd->Begin();
            m_RenderGraph->Execute(cmd);
            cmd->End();

            // Present
            m_Window->GetSwapchain()->Present();
             */
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
        return false;
    }
}
