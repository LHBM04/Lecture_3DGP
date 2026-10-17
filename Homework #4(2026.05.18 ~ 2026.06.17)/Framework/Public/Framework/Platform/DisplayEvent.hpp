#pragma once

#include "Event.hpp"

namespace TUK::Framework
{
	class Display;

	struct DisplayEvent : public Event
	{
	public:
		~DisplayEvent() noexcept override;

	protected:
		explicit DisplayEvent(Display* display) noexcept;

	public:
		Display* display = nullptr;
	};
}
