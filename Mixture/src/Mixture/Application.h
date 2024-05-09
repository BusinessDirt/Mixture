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

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline Window::Window& getWindow() { return *m_Window; }
		inline static Application& get() { return *s_Instance; }
	private:
		bool onWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window::Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}