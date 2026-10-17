#pragma once

#include "DisplayEvent.hpp"

namespace TUK::Framework
{
	class DisplayCloseEvent : public DisplayEvent
	{
	public:
		explicit DisplayCloseEvent(const Display* display) noexcept;
		~DisplayCloseEvent() noexcept override;
	};
}
