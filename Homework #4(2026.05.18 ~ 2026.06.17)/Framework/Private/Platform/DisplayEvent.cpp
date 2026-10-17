#include "Precompiled.hpp"
#include "Framework/Platform/DisplayEvent.hpp"

namespace TUK::Framework
{
	DisplayEvent::~DisplayEvent() noexcept
	{
	}

	const Display* DisplayEvent::GetDisplay() const noexcept
	{
		return display;
	}

	DisplayEvent::DisplayEvent(const Display* display) noexcept
		: display(display)
	{
	}
}
