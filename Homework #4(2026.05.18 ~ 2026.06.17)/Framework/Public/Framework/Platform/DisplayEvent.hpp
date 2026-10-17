#pragma once

#include "Event.hpp"

namespace TUK::Framework
{
	class Display;

	class DisplayEvent : public Event
	{
	public:
		~DisplayEvent() noexcept override;

		[[nodiscard]] const Display& GetDisplay() const noexcept;

		operator const Display&() const noexcept;

	protected:
		explicit DisplayEvent(const Display& display) noexcept;

	private:
		const Display& display;
	};
}
