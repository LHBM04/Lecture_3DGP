#include "Precompiled.hpp"
#include "Framework/Platform/DisplayMoveEvent.hpp"

namespace TUK::Framework
{
	DisplayMoveEvent::DisplayMoveEvent(const Display& display, const Vector2D<int>& position) noexcept
		: DisplayEvent(display)
		, position(position)
	{
	}

	DisplayMoveEvent::~DisplayMoveEvent() noexcept
	{
	}

	const Vector2D<int>& DisplayMoveEvent::GetPosition() const noexcept
	{
		return position;
	}
}
