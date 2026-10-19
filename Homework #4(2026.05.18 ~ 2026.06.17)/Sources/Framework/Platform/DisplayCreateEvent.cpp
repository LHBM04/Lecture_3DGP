#include "Precompiled.hpp"
#include "DisplayCreateEvent.hpp"

namespace TUK::Framework
{
	DisplayCreateEvent::DisplayCreateEvent(const Display& display) noexcept
		: DisplayEvent(display)
	{
	}

	DisplayCreateEvent::~DisplayCreateEvent() noexcept
	{
	}
}
