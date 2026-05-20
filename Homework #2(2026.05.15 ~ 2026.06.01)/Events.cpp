#include "Precompiled.h"
#include "Event.h"
#include "EventQueue.h"
#include "EventDispatcher.h"

EventDispatcher::EventDispatcher(EventQueue& queue_) noexcept
	: queue(queue_)
{
}

bool Event::IsHandled() const noexcept
{
	return handled;
}

void Event::SetHandled(bool value_) noexcept
{
	handled = value_;
}

void EventQueue::Clear() noexcept
{
	for (const std::function<void()>& clearer : clearers)
	{
		clearer();
	}
}
