#pragma once

#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	struct DisplayFocusEvent : public DisplayEvent
	{
		DisplayFocusEvent(Display* display, bool isFocused) noexcept;
		~DisplayFocusEvent() noexcept override;

		bool isFocused = false;
	};
}
