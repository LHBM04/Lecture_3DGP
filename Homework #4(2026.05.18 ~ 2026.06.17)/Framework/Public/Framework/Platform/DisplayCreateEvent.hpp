#pragma once

#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	struct DisplayCreateEvent : public DisplayEvent
	{
		explicit DisplayCreateEvent(Display* display) noexcept;
		~DisplayCreateEvent() noexcept override;
	};
}
