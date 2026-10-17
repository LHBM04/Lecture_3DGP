#pragma once

#include "../Math/Vector2D.hpp"
#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayResizeEvent : public DisplayEvent
	{
	public:
		DisplayResizeEvent(const Display* display, const Vector2D<int>& size) noexcept;
		~DisplayResizeEvent() noexcept override;

		[[nodiscard]] const Vector2D<int>& GetSize() const noexcept;

	private:
		Vector2D<int> size;
	};
}
