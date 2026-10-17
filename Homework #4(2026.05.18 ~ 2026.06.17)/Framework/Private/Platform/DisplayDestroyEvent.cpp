#include "Precompiled.hpp"
#include "Framework/Platform/DisplayDestroyEvent.hpp"

namespace TUK::Framework
{
	DisplayDestroyEvent::DisplayDestroyEvent(Display* display) noexcept
		: DisplayEvent(display)
	{
	}

	DisplayDestroyEvent::~DisplayDestroyEvent() noexcept
	{
	}
}
