#pragma once

#include "Window.h"

namespace Mixture {
	class Application {
	public:
		Application();
		virtual ~Application();
		
		void Run();
	private:
		std::unique_ptr<Window::Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();
}