#include "Precompiled.hpp"
#include "DisplayDestroyEvent.hpp"

namespace TUK::Framework
{
	DisplayDestroyEvent::DisplayDestroyEvent(const Display& display) noexcept
		: DisplayEvent(display)
	{
	}

	DisplayDestroyEvent::~DisplayDestroyEvent() noexcept
	{
	}
}
