#include "mxpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Logging/Log.h"

#include <GLFW/glfw3.h>

namespace Mixture {

	using namespace Events;

	Application::Application() {
		m_Window = std::unique_ptr<Window::Window>(Window::Window::create());
	}

	Application::~Application() {
		
	}

	void Application::Run() {
		while (m_Running) {
			m_Window->onUpdate();
		}
	}
}