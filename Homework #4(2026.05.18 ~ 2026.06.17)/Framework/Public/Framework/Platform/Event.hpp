#pragma once

namespace TUK::Framework
{
	class Event
	{
	public:
		virtual ~Event() noexcept;

		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;

		Event(Event&&) = delete;
		Event& operator=(Event&&) = delete;

	protected:
		Event() noexcept;
	};
}
