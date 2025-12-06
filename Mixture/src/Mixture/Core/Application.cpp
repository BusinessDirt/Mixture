#include "mxpch.hpp"
#include "Mixture/Core/Application.hpp"

#include "Mixture/Core/Time.hpp"

#include <Opal/Base.hpp>
#include <ranges>

namespace Mixture
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name, ApplicationCommandLineArgs args)
    {
        OPAL_ASSERT("Core", !s_Instance, "Mixture::Application::Application() - Application already exists!")
        
        s_Instance = this;

        auto props = WindowProps();
        props.Title = name;
        
        m_Window = CreateScope<Window>(props);
        m_Window->SetEventCallback(OPAL_BIND_EVENT_FN(OnEvent));
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
            m_Window->OnUpdate();
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
