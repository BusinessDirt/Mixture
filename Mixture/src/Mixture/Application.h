#pragma once


#include "Window.h"
#include "Mixture/LayerStack.h"
#include "Mixture/Events/ApplicationEvent.h"

namespace Mixture {
	class Application {
	public:
		Application();
		virtual ~Application();
		
		void run();

		void onEvent(Events::Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
	private:
		bool onWindowClose(Events::WindowCloseEvent& e);

		std::unique_ptr<Window::Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	Application* CreateApplication();
}