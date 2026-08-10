#include "mxpch.hpp"
#include "Mixture/Core/Window.hpp"

#include "Mixture/Core/Base.hpp"

#include "Mixture/Events/ApplicationEvent.hpp"
#include "Mixture/Events/KeyEvent.hpp"
#include "Mixture/Events/MouseEvent.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Mixture
{
	namespace
	{
		void GlfwErrorCallback(int error, const char* description)
		{
			OPAL_ERROR("Core/Window", "GLFW Error ({0}): {1}", error, description);
		}
	}

    Window::Window(const WindowProps& props)
    {
        if (props.Width <= 0 || props.Height <= 0)
            throw std::invalid_argument("Window dimensions must be greater than zero");
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
        m_Data.AspectRatio = static_cast<float>(props.Width) / static_cast<float>(props.Height);

		{
			OPAL_INFO("Core/Window", "Initializing GLFW");
			const int success = glfwInit();
			if (success != GLFW_TRUE) throw std::runtime_error("Could not initialize GLFW");
			glfwSetErrorCallback(GlfwErrorCallback);
		}

		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			m_WindowHandle = glfwCreateWindow(props.Width, props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			if (!m_WindowHandle)
            {
                glfwTerminate();
                throw std::runtime_error("Could not create a GLFW window");
            }
		}

		glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

		// CALLBACKS
		{
			glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, const int width, const int height)
				{
					WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
					data.Width = width;
					data.Height = height;
                    data.Minimized = width == 0 || height == 0;
                    if (!data.Minimized)
                        data.AspectRatio = static_cast<float>(width) / static_cast<float>(height);

					WindowResizeEvent event(width, height);
					data.EventCallback(event);
				});

            glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow* window, const int width, const int height)
                {
                    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                    data.Minimized = width == 0 || height == 0;

                    FramebufferResizeEvent event(width, height);
                    data.EventCallback(event);
                });

			glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window)
				{
					const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
					WindowCloseEvent event;
					data.EventCallback(event);
				});

			glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, const int key, int scancode, const int action, int mods)
				{
					const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

					switch (action)
					{
						case GLFW_PRESS:
						{
							KeyPressedEvent event(static_cast<KeyCode>(key));
							data.EventCallback(event);
							break;
						}
						case GLFW_RELEASE:
						{
							KeyReleasedEvent event(static_cast<KeyCode>(key));
							data.EventCallback(event);
							break;
						}
						case GLFW_REPEAT:
						{
							KeyPressedEvent event(static_cast<KeyCode>(key), true);
							data.EventCallback(event);
							break;
						}
						default: break;
					}
				});

			glfwSetCharCallback(m_WindowHandle, [](GLFWwindow* window, const unsigned int keycode)
				{
					const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

					KeyTypedEvent event(static_cast<KeyCode>(keycode));
					data.EventCallback(event);
				});

			glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, const int button, const int action, int mods)
				{
					const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

					switch (action)
					{
						case GLFW_PRESS:
						{
							MouseButtonPressedEvent event(static_cast<MouseCode>(button));
							data.EventCallback(event);
							break;
						}
						case GLFW_RELEASE:
						{
							MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
							data.EventCallback(event);
							break;
						}
						default: break;
					}
				});

			glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow* window, const double xOffset, const double yOffset)
				{
					const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

					MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
					data.EventCallback(event);
				});

			glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, const double xPos, const double yPos)
				{
					const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

					MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
					data.EventCallback(event);
				});
		}
    }

    Window::~Window()
    {
		if (m_WindowHandle) glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
    }

    void Window::OnUpdate() const
    {
		glfwPollEvents();
    }

    void Window::GetFramebufferSize(int* width, int* height) const
    {
		glfwGetFramebufferSize(m_WindowHandle, width, height);
    }
}
