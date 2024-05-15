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

	struct ApplicationCommandLineArgs {
		int count = 0;
		char** args = nullptr;

		const char* operator[](int index) const {
			MX_CORE_ASSERT(index < count);
			return args[index];
		}
	};

	class Application {
	public:
		Application(const std::string& name = "Mixture App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();
		

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		void close();

		ImGuiLayer* getImGuiLayer() { return m_ImGuiLayer; }

		Window& getWindow() { return *m_Window; }
		static Application& get() { return *s_Instance; }

		ApplicationCommandLineArgs getCommandLineArgs() const { return m_CommandLineArgs; }
	private:
		void run();
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);
	private:
		ApplicationCommandLineArgs m_CommandLineArgs;
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

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
