#pragma once

#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayDestroyEvent : public DisplayEvent
	{
	public:
		explicit DisplayDestroyEvent(const Display& display) noexcept;
		~DisplayDestroyEvent() noexcept override;
	};
}
