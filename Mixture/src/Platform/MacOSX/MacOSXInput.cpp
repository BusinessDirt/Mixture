#include "mxpch.hpp"

#ifdef MX_PLATFORM_MACOSX
#include "Mixture/Input/Input.hpp"
#include "Mixture/Core/Application.hpp"

#include <GLFW/glfw3.h>

namespace Mixture
{
	bool Input::IsKeyPressed(const KeyCode keycode) 
    {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button) 
    {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition() 
    {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX() {
		return GetMousePosition().x;
	}

	float Input::GetMouseY() {
		return GetMousePosition().y;
	}
}

#endif
