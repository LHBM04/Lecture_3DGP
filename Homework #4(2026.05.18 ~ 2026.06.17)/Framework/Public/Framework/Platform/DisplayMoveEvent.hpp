#pragma once

#include <cstdint>

#include "../Math/Vector2D.hpp"
#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayMoveEvent : public DisplayEvent
	{
	public:
		DisplayMoveEvent(const Display& display, const Vector2D<std::int32_t>& position) noexcept;
		~DisplayMoveEvent() noexcept override;

		[[nodiscard]] const Vector2D<std::int32_t>& GetPosition() const noexcept;

	private:
		Vector2D<std::int32_t> position;
	};
}
