#pragma once

#include "../Math/Vector2D.hpp"
#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	struct DisplayResizeEvent : public DisplayEvent
	{
		DisplayResizeEvent(Display* display, const Vector2D<int>& size) noexcept;
		~DisplayResizeEvent() noexcept override;

		Vector2D<int> size;
	};
}
