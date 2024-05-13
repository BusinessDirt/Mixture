#include "mxpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

#include "Mixture/Core/Base.h"

namespace Mixture {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) {
		MX_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::init() {
		MX_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		MX_CORE_ASSERT(status, "Failed to initialize Glad!");

		MX_CORE_INFO("OpenGL Info:");
		MX_CORE_INFO("|--[Vendor]    {0}", (const char*)glGetString(GL_VENDOR));
		MX_CORE_INFO("|--[Renderer]  {0}", (const char*)glGetString(GL_RENDERER));
		MX_CORE_INFO("+--[Version]   {0}", (const char*)glGetString(GL_VERSION));

		MX_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Mixture requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::swapBuffers() {
		MX_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}