#include "Application.h"

#include "Framework/Common/GLFWWindow.h"

namespace Mixture {

	Application::Application(const Window::Window::Properties& properties) {
		m_Window = new Window::GLFWWindow(properties);
	}

	Application::~Application() {
		delete m_Window;
	}

	void Application::Run() {
		while (!m_Window->shouldClose()) {
			m_Window->processEvents();
		}
	}
}