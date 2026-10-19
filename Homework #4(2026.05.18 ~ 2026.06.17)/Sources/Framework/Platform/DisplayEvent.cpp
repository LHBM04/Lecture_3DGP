#include "Precompiled.hpp"
#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	DisplayEvent::~DisplayEvent() noexcept
	{
	}

	const Display& DisplayEvent::GetDisplay() const noexcept
	{
		return display;
	}

	DisplayEvent::operator const Display&() const noexcept
	{
		return display;
	}

	DisplayEvent::DisplayEvent(const Display& display) noexcept
		: display(display)
	{
	}
}
