#include "Precompiled.hpp"
#include "Framework/Platform/DisplayEvent.hpp"

namespace TUK::Framework
{
	DisplayEvent::~DisplayEvent() noexcept
	{
	}

	DisplayEvent::DisplayEvent(Display* display) noexcept
		: display(display)
	{
	}
}
