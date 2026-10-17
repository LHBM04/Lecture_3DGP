#pragma once

#include "../Math/Vector2D.hpp"
#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayMoveEvent : public DisplayEvent
	{
	public:
		DisplayMoveEvent(const Display& display, const Vector2D<int>& position) noexcept;
		~DisplayMoveEvent() noexcept override;

		[[nodiscard]] const Vector2D<int>& GetPosition() const noexcept;

	private:
		Vector2D<int> position;
	};
}
