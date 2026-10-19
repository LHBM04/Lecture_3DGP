#pragma once

#include <cstdint>

#include "Framework/Math/Vector2D.hpp"
#include "Framework/Platform/DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayResizeEvent : public DisplayEvent
	{
	public:
		DisplayResizeEvent(const Display& display, const Vector2D<std::int32_t>& size) noexcept;
		~DisplayResizeEvent() noexcept override;

		[[nodiscard]] const Vector2D<std::int32_t>& GetSize() const noexcept;

	private:
		Vector2D<std::int32_t> size;
	};
}
