#include "mxpch.h"
#include "Mixture/Input/Input.h"

#include "Mixture/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Mixture {
	bool Input::isKeyPressed(const KeyCode keycode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS;
	}

	bool Input::isMouseButtonPressed(const MouseCode button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::getMousePosition() {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::getMouseX() {
		return getMousePosition().x;
	}

	float Input::getMouseY() {
		return getMousePosition().y;
	}
}
