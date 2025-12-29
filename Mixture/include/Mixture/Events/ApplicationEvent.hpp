#pragma once

/**
 * @file ApplicationEvent.hpp
 * @brief Application-specific events (resize, close, etc.).
 */

#include "Mixture/Events/Event.hpp"

#include <sstream>

namespace Mixture 
{
    /**
     * @brief Event triggered when the framebuffer is resized.
     */
    class FramebufferResizeEvent final : public Event
    {
    public:
        FramebufferResizeEvent(const unsigned int width, const unsigned int height) : m_Width(width), m_Height(height) {}

        OPAL_NODISCARD unsigned int GetWidth() const { return m_Width; }
        OPAL_NODISCARD unsigned int GetHeight() const { return m_Height; }

        OPAL_NODISCARD std::string ToString() const override
        {
            std::stringstream ss;
            ss << "FramebufferResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(FramebufferResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        unsigned int m_Width, m_Height;
    };

    /**
     * @brief Event triggered when the window is resized.
     */
	class WindowResizeEvent final : public Event
    {
	public:
		WindowResizeEvent(const unsigned int width, const unsigned int height) : m_Width(width), m_Height(height) {}

		OPAL_NODISCARD unsigned int GetWidth() const { return m_Width; }
		OPAL_NODISCARD unsigned int GetHeight() const { return m_Height; }

		OPAL_NODISCARD std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
	};

    /**
     * @brief Event triggered when the window is closed.
     */
	class WindowCloseEvent final : public Event 
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

    /**
     * @brief Event triggered every application tick.
     */
	class AppTickEvent final : public Event 
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

    /**
     * @brief Event triggered every application update.
     */
	class AppUpdateEvent final : public Event 
	{
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

    /**
     * @brief Event triggered every application render.
     */
	class AppRenderEvent final : public Event 
	{
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
