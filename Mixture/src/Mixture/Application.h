#pragma once

#include "Mixture/Events/ApplicationEvent.h"

#include "Window.h"

namespace Mixture {
	class Application {
	public:
		Application();
		virtual ~Application();
		
		void run();

		void onEvent(Events::Event& e);
	private:
		bool onWindowClose(Events::WindowCloseEvent& e);

		std::unique_ptr<Window::Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();
}