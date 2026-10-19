#include "Precompiled.hpp"
#include "DisplayResizeEvent.hpp"

namespace TUK::Framework
{
	DisplayResizeEvent::DisplayResizeEvent(const Display& display, const Vector2D<std::int32_t>& size) noexcept
		: DisplayEvent(display)
		, size(size)
	{
	}

	DisplayResizeEvent::~DisplayResizeEvent() noexcept
	{
	}

	const Vector2D<std::int32_t>& DisplayResizeEvent::GetSize() const noexcept
	{
		return size;
	}
}
