#include "Precompiled.h"
#include "EventQueue.h"

void EventQueue::Clear() noexcept
{
	for (const std::function<void()>& clearer : clearers)
	{
		clearer();
	}
}
