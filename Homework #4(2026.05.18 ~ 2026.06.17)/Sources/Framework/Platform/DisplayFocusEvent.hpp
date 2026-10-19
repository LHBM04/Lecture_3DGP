#pragma once

#include "Framework/Platform/DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayFocusEvent : public DisplayEvent
	{
	public:
		DisplayFocusEvent(const Display& display, bool isFocused) noexcept;
		~DisplayFocusEvent() noexcept override;

		[[nodiscard]] bool IsFocused() const noexcept;

	private:
		bool isFocused;
	};
}
