#include "Precompiled.hpp"
#include "Framework/Platform/DisplayResizeEvent.hpp"

namespace TUK::Framework
{
	DisplayResizeEvent::DisplayResizeEvent(Display* display, const Vector2D<int>& size) noexcept
		: DisplayEvent(display)
		, size(size)
	{
	}

	DisplayResizeEvent::~DisplayResizeEvent() noexcept
	{
	}
}
