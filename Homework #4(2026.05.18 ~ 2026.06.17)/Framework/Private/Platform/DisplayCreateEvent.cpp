#include "Precompiled.hpp"
#include "Framework/Platform/DisplayCreateEvent.hpp"

namespace TUK::Framework
{
	DisplayCreateEvent::DisplayCreateEvent(Display* display) noexcept
		: DisplayEvent(display)
	{
	}

	DisplayCreateEvent::~DisplayCreateEvent() noexcept
	{
	}
}
