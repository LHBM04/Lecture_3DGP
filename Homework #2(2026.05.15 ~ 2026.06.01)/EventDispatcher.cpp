#include "Precompiled.h"
#include "EventDispatcher.h"
#include "EventQueue.h"

EventDispatcher::EventDispatcher(EventQueue& queue_) noexcept
	: queue(queue_)
{
}
