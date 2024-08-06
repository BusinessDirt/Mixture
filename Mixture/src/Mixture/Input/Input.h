#pragma once

//#include <glm/glm.hpp>

#include "Mixture/Core/Base.h"

#include "Mixture/Input/KeyCodes.h"
#include "Mixture/Input/MouseCodes.h"

namespace Mixture 
{
	class Input 
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		//static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}