#include "GLFWWindow.h"

#include <unordered_map>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

void glfwErrorCallback(int error, const char* description) {
	// TODO
}

namespace Mixture::Window {
	GLFWWindow::GLFWWindow(const Window::Properties& properties) : Window(properties) {
		if (!glfwInit()) throw std::runtime_error("GLFW couldn't be initialized.");

		glfwSetErrorCallback(glfwErrorCallback);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		switch (properties.mode) {
			case Window::Mode::Fullscreen: {
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				m_WindowHandle = glfwCreateWindow(mode->width, mode->height, m_Properties.title.c_str(), monitor, NULL);
				break;
			}

			case Window::Mode::FullscreenBorderless: {
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwWindowHint(GLFW_RED_BITS, mode->redBits);
				glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
				glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
				m_WindowHandle = glfwCreateWindow(mode->width, mode->height, m_Properties.title.c_str(), monitor, NULL);
				break;
			}

			case Window::Mode::FullsreenStretch: {
				throw std::runtime_error("Cannot support stretch mode on this platform.");
				break;
			}

			default:
				m_WindowHandle = glfwCreateWindow(m_Properties.extent.width, m_Properties.extent.height, m_Properties.title.c_str(), NULL, NULL);
				break;
		}

		resize(Extent{ m_Properties.extent.width, m_Properties.extent.height });

		if (!m_WindowHandle) throw std::runtime_error("Couldn't create glfw window.");

		// TODO: Callbacks

	}

	GLFWWindow::~GLFWWindow() {
		glfwTerminate();
	}

	bool GLFWWindow::shouldClose() {
		return glfwWindowShouldClose(m_WindowHandle);
	}

	void GLFWWindow::processEvents() {
		glfwPollEvents();
	}

	void GLFWWindow::close() {
		glfwSetWindowShouldClose(m_WindowHandle, GLFW_TRUE);
	}

	/// @brief It calculates the dpi factor using the density from GLFW physical size
	/// <a href="https://www.glfw.org/docs/latest/monitor_guide.html#monitor_size">GLFW docs for dpi</a>
	float GLFWWindow::getDpiFactor() const{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(primaryMonitor);

		int widthMM, heightMM;
		glfwGetMonitorPhysicalSize(primaryMonitor, &widthMM, &heightMM);

		// As suggested by the GLFW monitor guide
		static const float inschToMM = 25.0f;
		static const float winBaseDensity = 96.0f;

		uint32_t dpi = static_cast<uint32_t>(vidmode->width / (widthMM / inschToMM));
		return dpi / winBaseDensity;
	}


	float GLFWWindow::getContentScaleFactor() const {
		int framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize(m_WindowHandle, &framebufferWidth, &framebufferHeight);
		int windowWidth, windowHeight;
		glfwGetWindowSize(m_WindowHandle, &windowWidth, &windowHeight);

		// We could return a 2D result here instead of a scalar,
		// but non-uniform scaling is very unlikely, and would
		// require significantly more changes in the IMGUI integration
		return static_cast<float>(framebufferWidth) / windowWidth;
	}
}