#pragma once

namespace Mixture 
{
    /**
     * @brief Enum representing all supported event types.
     */
	enum class EventType : uint8_t
	{
		None = 0,
		FramebufferResize, WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

    /**
     * @brief Bitfield enum for event categories.
     */
	enum EventCategory : uint8_t
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
	virtual EventType GetEventType() const override { return GetStaticType(); }\
	virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    /**
     * @brief Base class for all events.
     */
	class Event 
	{
	public:
		OPAL_NON_COPIABLE(Event);
		
		Event() = default;
		virtual ~Event() = default;

        /**
         * @brief Flag indicating if the event has been handled.
         */
		bool Handled = false;

		OPAL_NODISCARD virtual EventType GetEventType() const = 0;
		OPAL_NODISCARD virtual const char* GetName() const = 0;
		OPAL_NODISCARD virtual int GetCategoryFlags() const = 0;
		OPAL_NODISCARD virtual std::string ToString() const { return GetName(); }

        /**
         * @brief Checks if the event belongs to a specific category.
         * 
         * @param category The category to check.
         * @return true If the event is in the category.
         * @return false Otherwise.
         */
		OPAL_NODISCARD bool IsInCategory(const EventCategory category) const
		{
			return GetCategoryFlags() & category;
		}
	};

    /**
     * @brief Helper class to dispatch events based on their type.
     */
	class EventDispatcher 
	{
	public:
		explicit EventDispatcher(Event& event) : m_Event(event) {}

        /**
         * @brief Dispatches the event if it matches the template type T.
         * 
         * @tparam T The event type to match.
         * @tparam F The callback function type.
         * @param func The callback function to execute.
         * @return true If the event matched and the function was called.
         */
		template<typename T, typename F>
		bool Dispatch(const F& func) 
		{
			if (m_Event.GetEventType() == T::GetStaticType()) 
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};
}
