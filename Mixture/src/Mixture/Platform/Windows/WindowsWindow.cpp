#include "mxpch.h"
#include "Mixture/Platform/Windows/WindowsWindow.h"

#include "Mixture/Core/Core.h"
#include "Mixture/Events/ApplicationEvent.h"
#include "Mixture/Events/MouseEvent.h"
#include "Mixture/Events/KeyEvent.h"

#include "Mixture/Platform/OpenGL/OpenGLContext.h"

namespace Mixture {

#define MX_GET_WINDOW_DATA() WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window)

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description) {
		MX_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::create(const WindowProps& props) {
		return createScope<WindowsWindow>(props);
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

		if (s_GLFWWindowCount == 0) {
			MX_CORE_INFO("Initializing GLFW");
			int success = glfwInit();
			MX_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		m_Window = glfwCreateWindow((int)props.width, (int)props.height, m_Data.title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;

		m_Context = GraphicsContext::create(m_Window);
		m_Context->init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(true);

		// set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			MX_GET_WINDOW_DATA();
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			MX_GET_WINDOW_DATA();
			WindowCloseEvent event;
			data.eventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			MX_GET_WINDOW_DATA();

			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(key, 0);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event(key, 1);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
			MX_GET_WINDOW_DATA();

			KeyTypedEvent event(keycode);
			data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			MX_GET_WINDOW_DATA();

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			MX_GET_WINDOW_DATA();

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			MX_GET_WINDOW_DATA();

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.eventCallback(event);
		});
	}

	void WindowsWindow::shutdown() {
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0) {
			MX_CORE_INFO("Terminating GLFW");
			glfwTerminate();
		}
	}

	void WindowsWindow::onUpdate() {
		glfwPollEvents();
		m_Context->swapBuffers();
	}

	void WindowsWindow::setVSync(bool enabled) {
		glfwSwapInterval(enabled);
		m_Data.vSync = enabled;
	}

	bool WindowsWindow::isVSync() const {
		return m_Data.vSync;
	}
}