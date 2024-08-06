#include "mxpch.h"
#include "Application.h"

namespace Mixture
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, ApplicationCommandLineArgs args)
	{
		MX_CORE_ASSERT(!s_Instance, "Application already exisits!");
		s_Instance = this;

        m_Window = Window::Create(WindowProps(name));
        m_Window->SetEventCallback(MX_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{
		m_Window = nullptr;
	}

    void Application::Close()
	{
		m_Running = false;
	}

    void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(MX_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(MX_BIND_EVENT_FN(Application::OnWindowResize));

		// Handle other events
		
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();
		}
	}

    bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		this->Close();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		return false;
	}
}

