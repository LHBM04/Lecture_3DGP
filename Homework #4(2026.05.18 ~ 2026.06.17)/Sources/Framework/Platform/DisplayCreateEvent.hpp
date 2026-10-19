#pragma once

#include "Framework/Platform/DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayCreateEvent : public DisplayEvent
	{
	public:
		explicit DisplayCreateEvent(const Display& display) noexcept;
		~DisplayCreateEvent() noexcept override;
	};
}
