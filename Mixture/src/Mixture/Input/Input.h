#pragma once

#include <glm/glm.hpp>

#include "Mixture/Core/Base.h"
#include "Mixture/Input/KeyCodes.h"
#include "Mixture/Input/MouseCodes.h"

namespace Mixture {
	class Input {
	public:
		static bool isKeyPressed(KeyCode key);

		static bool isMouseButtonPressed(MouseCode button);
		static glm::vec2 getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};
}