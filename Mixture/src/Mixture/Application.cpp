#include "mxpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Logging/Log.h"
#include "Framework/Common/GLFWWindow.h"

namespace Mixture {

	using namespace Events;

	Application::Application(const Window::Window::Properties& properties) {
		m_Window = new Window::GLFWWindow(properties);
	}

	Application::~Application() {
		delete m_Window;
	}

	void Application::Run() {
		WindowResizeEvent e(1280, 720);
		if (e.isInCategory(EventCategoryApplication)) MX_TRACE(e.toString());
		if (e.isInCategory(EventCategoryInput)) MX_TRACE(e.toString());

		while (!m_Window->shouldClose()) {
			m_Window->processEvents();
		}
	}
}