#include "Precompiled.hpp"
#include "Framework/Platform/DisplayFocusEvent.hpp"

namespace TUK::Framework
{
	DisplayFocusEvent::DisplayFocusEvent(Display* display, bool isFocused) noexcept
		: DisplayEvent(display)
		, isFocused(isFocused)
	{
	}

	DisplayFocusEvent::~DisplayFocusEvent() noexcept
	{
	}
}
