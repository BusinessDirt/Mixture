#pragma once

namespace Mixture {
	typedef enum class KeyCode : uint16_t {
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode) {
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

#define MX_KEY_SPACE           ::Mixture::Key::Space
#define MX_KEY_APOSTROPHE      ::Mixture::Key::Apostrophe    /* ' */
#define MX_KEY_COMMA           ::Mixture::Key::Comma         /* , */
#define MX_KEY_MINUS           ::Mixture::Key::Minus         /* - */
#define MX_KEY_PERIOD          ::Mixture::Key::Period        /* . */
#define MX_KEY_SLASH           ::Mixture::Key::Slash         /* / */
#define MX_KEY_0               ::Mixture::Key::D0
#define MX_KEY_1               ::Mixture::Key::D1
#define MX_KEY_2               ::Mixture::Key::D2
#define MX_KEY_3               ::Mixture::Key::D3
#define MX_KEY_4               ::Mixture::Key::D4
#define MX_KEY_5               ::Mixture::Key::D5
#define MX_KEY_6               ::Mixture::Key::D6
#define MX_KEY_7               ::Mixture::Key::D7
#define MX_KEY_8               ::Mixture::Key::D8
#define MX_KEY_9               ::Mixture::Key::D9
#define MX_KEY_SEMICOLON       ::Mixture::Key::Semicolon     /* ; */
#define MX_KEY_EQUAL           ::Mixture::Key::Equal         /* = */
#define MX_KEY_A               ::Mixture::Key::A
#define MX_KEY_B               ::Mixture::Key::B
#define MX_KEY_C               ::Mixture::Key::C
#define MX_KEY_D               ::Mixture::Key::D
#define MX_KEY_E               ::Mixture::Key::E
#define MX_KEY_F               ::Mixture::Key::F
#define MX_KEY_G               ::Mixture::Key::G
#define MX_KEY_H               ::Mixture::Key::H
#define MX_KEY_I               ::Mixture::Key::I
#define MX_KEY_J               ::Mixture::Key::J
#define MX_KEY_K               ::Mixture::Key::K
#define MX_KEY_L               ::Mixture::Key::L
#define MX_KEY_M               ::Mixture::Key::M
#define MX_KEY_N               ::Mixture::Key::N
#define MX_KEY_O               ::Mixture::Key::O
#define MX_KEY_P               ::Mixture::Key::P
#define MX_KEY_Q               ::Mixture::Key::Q
#define MX_KEY_R               ::Mixture::Key::R
#define MX_KEY_S               ::Mixture::Key::S
#define MX_KEY_T               ::Mixture::Key::T
#define MX_KEY_U               ::Mixture::Key::U
#define MX_KEY_V               ::Mixture::Key::V
#define MX_KEY_W               ::Mixture::Key::W
#define MX_KEY_X               ::Mixture::Key::X
#define MX_KEY_Y               ::Mixture::Key::Y
#define MX_KEY_Z               ::Mixture::Key::Z
#define MX_KEY_LEFT_BRACKET    ::Mixture::Key::LeftBracket   /* [ */
#define MX_KEY_BACKSLASH       ::Mixture::Key::Backslash     /* \ */
#define MX_KEY_RIGHT_BRACKET   ::Mixture::Key::RightBracket  /* ] */
#define MX_KEY_GRAVE_ACCENT    ::Mixture::Key::GraveAccent   /* ` */
#define MX_KEY_WORLD_1         ::Mixture::Key::World1        /* non-US #1 */
#define MX_KEY_WORLD_2         ::Mixture::Key::World2        /* non-US #2 */

/* Function keys */
#define MX_KEY_ESCAPE          ::Mixture::Key::Escape
#define MX_KEY_ENTER           ::Mixture::Key::Enter
#define MX_KEY_TAB             ::Mixture::Key::Tab
#define MX_KEY_BACKSPACE       ::Mixture::Key::Backspace
#define MX_KEY_INSERT          ::Mixture::Key::Insert
#define MX_KEY_DELETE          ::Mixture::Key::Delete
#define MX_KEY_RIGHT           ::Mixture::Key::Right
#define MX_KEY_LEFT            ::Mixture::Key::Left
#define MX_KEY_DOWN            ::Mixture::Key::Down
#define MX_KEY_UP              ::Mixture::Key::Up
#define MX_KEY_PAGE_UP         ::Mixture::Key::PageUp
#define MX_KEY_PAGE_DOWN       ::Mixture::Key::PageDown
#define MX_KEY_HOME            ::Mixture::Key::Home
#define MX_KEY_END             ::Mixture::Key::End
#define MX_KEY_CAPS_LOCK       ::Mixture::Key::CapsLock
#define MX_KEY_SCROLL_LOCK     ::Mixture::Key::ScrollLock
#define MX_KEY_NUM_LOCK        ::Mixture::Key::NumLock
#define MX_KEY_PRINT_SCREEN    ::Mixture::Key::PrintScreen
#define MX_KEY_PAUSE           ::Mixture::Key::Pause
#define MX_KEY_F1              ::Mixture::Key::F1
#define MX_KEY_F2              ::Mixture::Key::F2
#define MX_KEY_F3              ::Mixture::Key::F3
#define MX_KEY_F4              ::Mixture::Key::F4
#define MX_KEY_F5              ::Mixture::Key::F5
#define MX_KEY_F6              ::Mixture::Key::F6
#define MX_KEY_F7              ::Mixture::Key::F7
#define MX_KEY_F8              ::Mixture::Key::F8
#define MX_KEY_F9              ::Mixture::Key::F9
#define MX_KEY_F10             ::Mixture::Key::F10
#define MX_KEY_F11             ::Mixture::Key::F11
#define MX_KEY_F12             ::Mixture::Key::F12
#define MX_KEY_F13             ::Mixture::Key::F13
#define MX_KEY_F14             ::Mixture::Key::F14
#define MX_KEY_F15             ::Mixture::Key::F15
#define MX_KEY_F16             ::Mixture::Key::F16
#define MX_KEY_F17             ::Mixture::Key::F17
#define MX_KEY_F18             ::Mixture::Key::F18
#define MX_KEY_F19             ::Mixture::Key::F19
#define MX_KEY_F20             ::Mixture::Key::F20
#define MX_KEY_F21             ::Mixture::Key::F21
#define MX_KEY_F22             ::Mixture::Key::F22
#define MX_KEY_F23             ::Mixture::Key::F23
#define MX_KEY_F24             ::Mixture::Key::F24
#define MX_KEY_F25             ::Mixture::Key::F25

/* Keypad */
#define MX_KEY_KP_0            ::Mixture::Key::KP0
#define MX_KEY_KP_1            ::Mixture::Key::KP1
#define MX_KEY_KP_2            ::Mixture::Key::KP2
#define MX_KEY_KP_3            ::Mixture::Key::KP3
#define MX_KEY_KP_4            ::Mixture::Key::KP4
#define MX_KEY_KP_5            ::Mixture::Key::KP5
#define MX_KEY_KP_6            ::Mixture::Key::KP6
#define MX_KEY_KP_7            ::Mixture::Key::KP7
#define MX_KEY_KP_8            ::Mixture::Key::KP8
#define MX_KEY_KP_9            ::Mixture::Key::KP9
#define MX_KEY_KP_DECIMAL      ::Mixture::Key::KPDecimal
#define MX_KEY_KP_DIVIDE       ::Mixture::Key::KPDivide
#define MX_KEY_KP_MULTIPLY     ::Mixture::Key::KPMultiply
#define MX_KEY_KP_SUBTRACT     ::Mixture::Key::KPSubtract
#define MX_KEY_KP_ADD          ::Mixture::Key::KPAdd
#define MX_KEY_KP_ENTER        ::Mixture::Key::KPEnter
#define MX_KEY_KP_EQUAL        ::Mixture::Key::KPEqual

#define MX_KEY_LEFT_SHIFT      ::Mixture::Key::LeftShift
#define MX_KEY_LEFT_CONTROL    ::Mixture::Key::LeftControl
#define MX_KEY_LEFT_ALT        ::Mixture::Key::LeftAlt
#define MX_KEY_LEFT_SUPER      ::Mixture::Key::LeftSuper
#define MX_KEY_RIGHT_SHIFT     ::Mixture::Key::RightShift
#define MX_KEY_RIGHT_CONTROL   ::Mixture::Key::RightControl
#define MX_KEY_RIGHT_ALT       ::Mixture::Key::RightAlt
#define MX_KEY_RIGHT_SUPER     ::Mixture::Key::RightSuper
#define MX_KEY_MENU            ::Mixture::Key::Menu