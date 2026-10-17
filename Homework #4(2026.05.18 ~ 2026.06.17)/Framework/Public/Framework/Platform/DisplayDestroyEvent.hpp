#pragma once

#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	struct DisplayDestroyEvent : public DisplayEvent
	{
		explicit DisplayDestroyEvent(Display* display) noexcept;
		~DisplayDestroyEvent() noexcept override;
	};
}
