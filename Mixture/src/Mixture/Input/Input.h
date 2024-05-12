#pragma once

#include "Mixture/Core/Base.h"
#include "Mixture/Input/KeyCodes.h"
#include "Mixture/Input/MouseCodes.h"

namespace Mixture {
	class Input {
	public:
		static bool isKeyPressed(KeyCode key);

		static bool isMouseButtonPressed(MouseCode button);
		static std::pair<float, float> getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};
}