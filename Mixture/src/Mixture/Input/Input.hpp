#pragma once

#include <glm/glm.hpp>

#include "Mixture/Core/Base.hpp"

#include "Mixture/Input/KeyCodes.hpp"
#include "Mixture/Input/MouseCodes.hpp"

namespace Mixture 
{
	class Input 
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
