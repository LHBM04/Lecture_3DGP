#pragma once

#include "../Math/Vector2D.hpp"
#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	struct DisplayMoveEvent : public DisplayEvent
	{
		DisplayMoveEvent(Display* display, const Vector2D<int>& position) noexcept;
		~DisplayMoveEvent() noexcept override;

		Vector2D<int> position;
	};
}
