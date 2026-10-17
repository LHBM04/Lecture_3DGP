#pragma once

#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayCreateEvent : public DisplayEvent
	{
	public:
		explicit DisplayCreateEvent(const Display& display) noexcept;
		~DisplayCreateEvent() noexcept override;
	};
}
