#include "mxpch.h"
#include "Application.h"

#include "Logging/Log.h"

#include <GLFW/glfw3.h>

namespace Mixture {

	using namespace Events;

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application() {
		m_Window = std::unique_ptr<Window::Window>(Window::Window::create());
		m_Window->setEventCallback(BIND_EVENT_FN(onEvent));
	}

	Application::~Application() {
		
	}

	void Application::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

		MX_CORE_TRACE("{0}", e.toString());
	}

	void Application::run() {
		while (m_Running) {
			m_Window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}
}