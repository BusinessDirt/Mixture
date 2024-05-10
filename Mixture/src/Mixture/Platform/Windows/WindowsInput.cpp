#include "mxpch.h"
#include "WindowsInput.h"

#include "Mixture/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Mixture {
	Scope<Input> Input::s_Instance = createScope<WindowsInput>();

	bool WindowsInput::isKeyPressedImpl(int keycode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::isMouseButtonPressedImpl(int button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::getMousePositionImpl() {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::getMouseXImpl() {
		return getMousePositionImpl().first;
	}

	float WindowsInput::getMouseYImpl() {
		return getMousePositionImpl().second;
	}
}