#include "mxpch.h"

#include "Mixture/Core.h"

#include "WindowsWindow.h"

namespace Mixture::Window {
	static bool s_GLFWInitialized = false;

	Window* Window::create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		init(props);
	}

	WindowsWindow::~WindowsWindow() {
		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props) {
		m_Data.title = props.title;
		m_Data.width = props.width;
		m_Data.height = props.height;

		MX_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			MX_CORE_ASSERT(success, "Could not intialize GLFW!");

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.width, (int)props.height, m_Data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(true);
	}

	void WindowsWindow::shutdown() {
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::onUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::setVSync(bool enabled) {
		glfwSwapInterval(enabled);
		m_Data.vSync = enabled;
	}

	bool WindowsWindow::isVSync() const {
		return m_Data.vSync;
	}
}