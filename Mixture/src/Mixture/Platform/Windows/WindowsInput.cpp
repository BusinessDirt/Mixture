#include "mxpch.h"
#include "Mixture/Platform/Windows/WindowsInput.h"

#include "Mixture/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Mixture {
	bool WindowsInput::isKeyPressedImpl(KeyCode keycode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::isMouseButtonPressedImpl(MouseCode button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
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