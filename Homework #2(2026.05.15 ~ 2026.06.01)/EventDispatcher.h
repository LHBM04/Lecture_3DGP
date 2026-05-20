#pragma once

#include "EventQueue.h"

class EventDispatcher
{
public:
	explicit EventDispatcher(EventQueue& queue_) noexcept;

	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;

	EventDispatcher(EventDispatcher&&) = delete;
	EventDispatcher& operator=(EventDispatcher&&) = delete;

	template <std::derived_from<Event> TEvent, class TFunction>
	void Dispatch(TFunction&& function_);

private:
	EventQueue& queue;
};

template <std::derived_from<Event> TEvent, class TFunction>
inline void EventDispatcher::Dispatch(TFunction&& function_)
{
	queue.ForEach<TEvent>(
		[&](TEvent& event_)
		{
			std::invoke(std::forward<TFunction>(function_), event_);
		});
}
