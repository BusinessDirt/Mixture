#pragma once

#include "Mixture/Core/Base.h"

#include "Mixture/Core/Window.h"

#include "Mixture/Events/Event.h"
#include "Mixture/Events/ApplicationEvent.h"
#include "Mixture/Events/KeyEvent.h"
#include "Mixture/Events/MouseEvent.h"

#include "Mixture/Renderer/Renderer.h"

int Entrypoint(int argc, char** argv);

namespace Mixture
{

	struct ApplicationCommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		const char* operator[](int index) const
		{
			MX_CORE_ASSERT(index < count);
			return args[index];
		}
	};

	class Application
	{
	public:
		Application(const std::string& name = "Onyx App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Close();
		void OnEvent(Event& e);

		static Application& Get() { return *s_Instance; }
		const Window& GetWindow() const { return *m_Window; }

	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Scope<Window> m_Window;
		bool m_Running = true;

	private:
		static Application* s_Instance;
		friend int ::Entrypoint(int argc, char** argv);
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}