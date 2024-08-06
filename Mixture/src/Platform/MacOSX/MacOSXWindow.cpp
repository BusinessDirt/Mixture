#include "mxpch.h"
#include "MacOSXWindow.h"

#include "Mixture/Core/Base.h"

#include "Mixture/Events/ApplicationEvent.h"
#include "Mixture/Events/MouseEvent.h"
#include "Mixture/Events/KeyEvent.h"

namespace Mixture
{

    static void GLFWErrorCallback(int error, const char* description)
	{
		MX_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

    MacOSXWindow::MacOSXWindow(const WindowProps& props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        MX_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        {
            MX_CORE_INFO("Initializing GLFW");
            int success = glfwInit();
            MX_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);

        // CALLBACKS
		{
			glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					data.Width = width;
					data.Height = height;
					
					if (width == 0 || height == 0) data.Minimized = true;

					WindowResizeEvent event(width, height);
					data.EventCallback(event);
				});

			glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					WindowCloseEvent event;
					data.EventCallback(event);
				});

			glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

					switch (action)
					{
						case GLFW_PRESS:
						{
							KeyPressedEvent event(key);
							data.EventCallback(event);
							break;
						}
						case GLFW_RELEASE:
						{
							KeyReleasedEvent event(key);
							data.EventCallback(event);
							break;
						}
						case GLFW_REPEAT:
						{
							KeyPressedEvent event(key, true);
							data.EventCallback(event);
							break;
						}
					}
				});

			glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) 
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

					KeyTypedEvent event(keycode);
					data.EventCallback(event);
				});

			glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

					switch (action)
					{
						case GLFW_PRESS:
						{
							MouseButtonPressedEvent event(button);
							data.EventCallback(event);
							break;
						}
						case GLFW_RELEASE:
						{
							MouseButtonReleasedEvent event(button);
							data.EventCallback(event);
							break;
						}
					}
				});

			glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) 
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

					MouseScrolledEvent event((float)xOffset, (float)yOffset);
					data.EventCallback(event);
				});

			glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) 
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

					MouseMovedEvent event((float)xPos, (float)yPos);
					data.EventCallback(event);
				});
		}
    }

    MacOSXWindow::~MacOSXWindow()
    {
        glfwDestroyWindow(m_Window);
        MX_CORE_INFO("Terminating GLFW");
        glfwTerminate();
    }

    void MacOSXWindow::OnUpdate()
    {
        glfwPollEvents();
    }
}