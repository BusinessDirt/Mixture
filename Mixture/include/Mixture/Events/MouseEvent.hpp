#pragma once

#include "Mixture/Events/Event.hpp"
#include "Mixture/Input/MouseCodes.hpp"

namespace Mixture 
{

    /**
     * @brief Event triggered when the mouse moves.
     */
	class MouseMovedEvent final : public Event 
	{
	public:
		MouseMovedEvent(const float x, const float y) : m_MouseX(x), m_MouseY(y) {}

		OPAL_NODISCARD float GetX() const { return m_MouseX; }
		OPAL_NODISCARD float GetY() const { return m_MouseY; }

		OPAL_NODISCARD std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

    /**
     * @brief Event triggered when the mouse is scrolled.
     */
	class MouseScrolledEvent final : public Event 
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

		OPAL_NODISCARD float GetXOffset() const { return m_XOffset; }
		OPAL_NODISCARD float GetYOffset() const { return m_YOffset; }

		OPAL_NODISCARD std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};

    /**
     * @brief Base class for mouse button events.
     */
	class MouseButtonEvent : public Event 
	{
	public:
		OPAL_NODISCARD MouseCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		explicit MouseButtonEvent(const MouseCode button) : m_Button(button) {}

		MouseCode m_Button;
	};

    /**
     * @brief Event triggered when a mouse button is pressed.
     */
	class MouseButtonPressedEvent final : public MouseButtonEvent 
	{
	public:
		explicit MouseButtonPressedEvent(const MouseCode button) : MouseButtonEvent(button) {}

		OPAL_NODISCARD std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

    /**
     * @brief Event triggered when a mouse button is released.
     */
	class MouseButtonReleasedEvent final : public MouseButtonEvent 
	{
	public:
		explicit MouseButtonReleasedEvent(const MouseCode button) : MouseButtonEvent(button) {}

		OPAL_NODISCARD std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}