#include "mxpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

#include "Mixture/Core.h"

namespace Mixture {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) {
		MX_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::init() {
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		MX_CORE_ASSERT(status, "Failed to initialize Glad!");

		MX_CORE_INFO("OpenGL Info:");
		MX_CORE_INFO("|--[Vendor]    {0}", (const char*)glGetString(GL_VENDOR));
		MX_CORE_INFO("|--[Renderer]  {0}", (const char*)glGetString(GL_RENDERER));
		MX_CORE_INFO("+--[Version]   {0}", (const char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::swapBuffers() {
		glfwSwapBuffers(m_WindowHandle);
	}
}