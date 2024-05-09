#pragma once


#include "Window.h"
#include "Mixture/LayerStack.h"
#include "Mixture/Events/ApplicationEvent.h"

#include "Mixture/ImGui/ImGuiLayer.h"

#include "Mixture/Renderer/Shader.h"
#include "Mixture/Renderer/Buffer.h"
#include "Mixture/Renderer/VertexArray.h"

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

		std::unique_ptr<Window::Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexArray> m_VertexArray;

		std::unique_ptr<Shader> m_BlueShader;
		std::unique_ptr<VertexArray> m_SquareVA;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}