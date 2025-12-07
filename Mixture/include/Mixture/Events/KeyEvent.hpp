#pragma once

#include "Mixture/Events/Event.hpp"
#include "Mixture/Input/KeyCodes.hpp"

namespace Mixture 
{
    /**
     * @brief Base class for key events.
     */
	class KeyEvent : public Event 
	{
	public:
		OPAL_NODISCARD KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		explicit KeyEvent(const KeyCode keycode) : m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

    /**
     * @brief Event triggered when a key is pressed.
     */
	class KeyPressedEvent final : public KeyEvent 
	{
	public:
		explicit KeyPressedEvent(const KeyCode keycode, const bool isRepeat = false) : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

		OPAL_NODISCARD bool IsRepeat() const { return m_IsRepeat; }

		OPAL_NODISCARD std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeat;
	};

    /**
     * @brief Event triggered when a key is released.
     */
	class KeyReleasedEvent final : public KeyEvent 
	{
	public:
		explicit KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

		OPAL_NODISCARD std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

    /**
     * @brief Event triggered when a character is typed (text input).
     */
	class KeyTypedEvent final : public KeyEvent 
	{
	public:
		explicit KeyTypedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

		OPAL_NODISCARD std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}