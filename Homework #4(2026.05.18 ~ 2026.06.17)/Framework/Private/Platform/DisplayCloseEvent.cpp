#include "Precompiled.hpp"
#include "Framework/Platform/DisplayCloseEvent.hpp"

namespace TUK::Framework
{
	DisplayCloseEvent::DisplayCloseEvent(Display* display) noexcept
		: DisplayEvent(display)
	{
	}

	DisplayCloseEvent::~DisplayCloseEvent() noexcept
	{
	}
}
