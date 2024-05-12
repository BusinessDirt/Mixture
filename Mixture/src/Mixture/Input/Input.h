#pragma once

#include "Mixture/Core/Base.h"
#include "Mixture/Input/KeyCodes.h"
#include "Mixture/Input/MouseCodes.h"

namespace Mixture {
	class Input {
	protected:
		Input() = default;
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		static bool isKeyPressed(KeyCode key) { return s_Instance->isKeyPressedImpl(key); }
		static bool isMouseButtonPressed(MouseCode button) { return s_Instance->isMouseButtonPressedImpl(button); }
		static std::pair<float, float> getMousePosition() { return s_Instance->getMousePositionImpl(); }
		static float getMouseX() { return s_Instance->getMouseXImpl(); }
		static float getMouseY() { return s_Instance->getMouseYImpl(); }

		static Scope<Input> create();
	protected:
		virtual bool isKeyPressedImpl(KeyCode key) = 0;
		virtual bool isMouseButtonPressedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> getMousePositionImpl() = 0;
		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;
	private:
		static Scope<Input> s_Instance;
	};
}