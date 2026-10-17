#include "Precompiled.hpp"
#include "Framework/Platform/DisplayMoveEvent.hpp"

namespace TUK::Framework
{
	DisplayMoveEvent::DisplayMoveEvent(Display* display, const Vector2D<int>& position) noexcept
		: DisplayEvent(display)
		, position(position)
	{
	}

	DisplayMoveEvent::~DisplayMoveEvent() noexcept
	{
	}
}
