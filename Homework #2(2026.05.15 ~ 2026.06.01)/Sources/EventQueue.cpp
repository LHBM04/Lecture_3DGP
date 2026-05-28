#include "Precompiled.h"
#include "EventQueue.h"

void EventQueue::Clear() noexcept
{
	events.clear();
}

bool EventQueue::IsEmpty() const noexcept
{
	return events.empty();
}

std::size_t EventQueue::GetCount() const noexcept
{
	return events.size();
}

std::span<std::unique_ptr<Event>> EventQueue::GetEvents() noexcept
{
	return events;
}

std::span<const std::unique_ptr<Event>> EventQueue::GetEvents() const noexcept
{
	return events;
}
