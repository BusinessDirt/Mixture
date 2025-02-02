#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Core/Window.hpp"
#include "Mixture/Core/Timestep.hpp"

#include "Mixture/Events/Event.hpp"
#include "Mixture/Events/ApplicationEvent.hpp"
#include "Mixture/Events/KeyEvent.hpp"
#include "Mixture/Events/MouseEvent.hpp"

#include "Mixture/Renderer/Renderer.hpp"
#include "Mixture/Assets/AssetManager.hpp"

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
		const AssetManager& GetAssetManager() const { return *m_AssetManager; }
		const Timestep& GetTimestep() const { return m_Timestep; }

	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Scope<Window> m_Window;
        Scope<AssetManager> m_AssetManager;
		bool m_Running = true;
		Timestep m_Timestep;

	private:
		static Application* s_Instance;
		friend int ::Entrypoint(int argc, char** argv);
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
