#pragma once
#include "hzpch.h"

#include "Hazel/Core/Base.h"

// now in pch
//#include <string>
//#include <functional>

namespace Hazel {

	// Events in Hazel are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication    = BIT(0),   // 1 << 0   00001
		EventCategoryInput          = BIT(1),   // 1 << 1   00010
		EventCategoryKeyboard       = BIT(2),   // 1 << 2   00100
		EventCategoryMouse          = BIT(3),   // 1 << 3   01000
		EventCategoryMouseButton    = BIT(4)    // 1 << 4   10000
	};

	// This will add the following events when it is used in the derived classes.
	// GetStaticType() allows us to avoid needing var storage for the type (of the derived class).
	//    It is static because we don't need > one per derived class.
	// GetEventType() allows us to get (for comparison sake) the EventType
	// GetName() turns the EventType into a string
	// 
	// ## is a concatenation operator for macros
	// # is the stringify operator for macros


#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class HAZEL_API Event
	{
		friend class EventDispatcher;
	public:
		virtual ~Event() = default;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
		bool Handled = false;

	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);				// original checkin
				// m_Event.Handled |= func(static_cast<T&>(m_Event));	// from the most recent checkin
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}

