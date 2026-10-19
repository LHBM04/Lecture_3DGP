#include "Precompiled.hpp"
#include "DisplayCloseEvent.hpp"

namespace TUK::Framework
{
	DisplayCloseEvent::DisplayCloseEvent(const Display& display) noexcept
		: DisplayEvent(display)
	{
	}

	DisplayCloseEvent::~DisplayCloseEvent() noexcept
	{
	}
}
