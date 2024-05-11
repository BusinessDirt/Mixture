#pragma once

namespace Mixture {
	typedef enum class MouseCode : uint16_t {
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode) {
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define MX_MOUSE_BUTTON_0      ::Mixture::Mouse::Button0
#define MX_MOUSE_BUTTON_1      ::Mixture::Mouse::Button1
#define MX_MOUSE_BUTTON_2      ::Mixture::Mouse::Button2
#define MX_MOUSE_BUTTON_3      ::Mixture::Mouse::Button3
#define MX_MOUSE_BUTTON_4      ::Mixture::Mouse::Button4
#define MX_MOUSE_BUTTON_5      ::Mixture::Mouse::Button5
#define MX_MOUSE_BUTTON_6      ::Mixture::Mouse::Button6
#define MX_MOUSE_BUTTON_7      ::Mixture::Mouse::Button7
#define MX_MOUSE_BUTTON_LAST   ::Mixture::Mouse::ButtonLast
#define MX_MOUSE_BUTTON_LEFT   ::Mixture::Mouse::ButtonLeft
#define MX_MOUSE_BUTTON_RIGHT  ::Mixture::Mouse::ButtonRight
#define MX_MOUSE_BUTTON_MIDDLE ::Mixture::Mouse::ButtonMiddle