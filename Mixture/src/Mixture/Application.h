#pragma once

#include "Framework/Common/Window.h"

namespace Mixture {
	class Application {
	public:
		Application(const Window::Window::Properties& properties);
		virtual ~Application();
		
		void Run();
	private:
		Window::Window* m_Window;
	};

	Application* CreateApplication();
}