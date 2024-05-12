#pragma once


#include "Window.h"

#include "Mixture/Core/Base.h"

#include "Mixture/Core/Window.h"
#include "Mixture/Core/LayerStack.h"
#include "Mixture/Events/Event.h"
#include "Mixture/Events/ApplicationEvent.h"
#include "Mixture/Core/Timestep.h"

#include "Mixture/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Mixture {
	class Application {
	public:
		Application(const std::string& name = "Mixture App");
		virtual ~Application();
		

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		void close();

		Window& getWindow() { return *m_Window; }
		static Application& get() { return *s_Instance; }
	private:
		void run();
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}