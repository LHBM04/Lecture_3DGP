#include "Precompiled.hpp"
#include "Framework/Platform/DisplayMoveEvent.hpp"

namespace TUK::Framework
{
	DisplayMoveEvent::DisplayMoveEvent(const Display& display, const Vector2D<std::int32_t>& position) noexcept
		: DisplayEvent(display)
		, position(position)
	{
	}

	DisplayMoveEvent::~DisplayMoveEvent() noexcept
	{
	}

	const Vector2D<std::int32_t>& DisplayMoveEvent::GetPosition() const noexcept
	{
		return position;
	}
}
