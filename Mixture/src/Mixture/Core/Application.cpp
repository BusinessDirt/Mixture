#include "mxpch.h"
#include "Mixture/Core/Application.h"

#include "Mixture/Logging/Log.h"
#include "Mixture/Renderer/Renderer.h"
#include "Mixture/Input/Input.h"

#include <GLFW/glfw3.h>

namespace Mixture {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		MX_PROFILE_FUNCTION();

		MX_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::create();
		m_Window->setEventCallback(MX_BIND_EVENT_FN(Application::onEvent));

		Renderer::init();

		m_ImGuiLayer = new ImGuiLayer();
		pushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {
		MX_PROFILE_FUNCTION();

		Renderer::shutdown();
	}

	void Application::pushLayer(Layer* layer) {
		MX_PROFILE_FUNCTION();

		m_LayerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* layer) {
		MX_PROFILE_FUNCTION();

		m_LayerStack.pushOverlay(layer);
	}

	void Application::close() {
		m_Running = false;
	}

	void Application::onEvent(Event& e) {
		MX_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(MX_BIND_EVENT_FN(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(MX_BIND_EVENT_FN(Application::onWindowResize));

		for (std::vector<Layer*>::reverse_iterator it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			if (e.handled) break;
			(*it)->onEvent(e);
		}
	}

	void Application::run() {
		MX_PROFILE_FUNCTION();

		while (m_Running) {
			MX_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) {
				{
					MX_PROFILE_SCOPE("LayerStack onUpdate");
					for (Layer* layer : m_LayerStack)
						layer->onUpdate(timestep);
				}
				m_ImGuiLayer->begin();
				{
					MX_PROFILE_SCOPE("LayerStack onImGuiRender");
					for (Layer* layer : m_LayerStack)
						layer->onImGuiRender();
				}
				m_ImGuiLayer->end();
			}
			

			m_Window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& e) {
		MX_PROFILE_FUNCTION();

		if (e.getWidth() == 0 || e.getHeight() == 0) {
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::onWindowResize(e.getWidth(), e.getHeight());

		return false;
	}
}