#include "Precompiled.hpp"
#include "Framework/Platform/DisplayFocusEvent.hpp"

namespace TUK::Framework
{
	DisplayFocusEvent::DisplayFocusEvent(const Display& display, bool isFocused) noexcept
		: DisplayEvent(display)
		, isFocused(isFocused)
	{
	}

	DisplayFocusEvent::~DisplayFocusEvent() noexcept
	{
	}

	bool DisplayFocusEvent::IsFocused() const noexcept
	{
		return isFocused;
	}
}
