#pragma once

#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	struct DisplayCloseEvent : public DisplayEvent
	{
		explicit DisplayCloseEvent(Display* display) noexcept;
		~DisplayCloseEvent() noexcept override;
	};
}
