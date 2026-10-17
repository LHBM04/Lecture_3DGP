#include "Precompiled.hpp"
#include "Framework/Platform/DisplayResizeEvent.hpp"

namespace TUK::Framework
{
	DisplayResizeEvent::DisplayResizeEvent(const Display* display, const Vector2D<int>& size) noexcept
		: DisplayEvent(display)
		, size(size)
	{
	}

	DisplayResizeEvent::~DisplayResizeEvent() noexcept
	{
	}

	const Vector2D<int>& DisplayResizeEvent::GetSize() const noexcept
	{
		return size;
	}
}
