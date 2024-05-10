#pragma once


#include "Window.h"
#include "Mixture/LayerStack.h"
#include "Mixture/Events/ApplicationEvent.h"

#include "Mixture/Core/Timestep.h"

#include "Mixture/ImGui/ImGuiLayer.h"

namespace Mixture {
	class Application {
	public:
		Application();
		virtual ~Application() = default;
		
		void run();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline Window::Window& getWindow() { return *m_Window; }
		inline static Application& get() { return *s_Instance; }
	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window::Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}