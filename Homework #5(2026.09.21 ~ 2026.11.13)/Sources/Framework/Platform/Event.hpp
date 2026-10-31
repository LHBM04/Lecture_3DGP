#pragma once

namespace TUK::Framework
{
	struct Event
	{
		Event() noexcept = default;
		virtual ~Event() noexcept = default;

		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;

		Event(Event&&) = delete;
		Event& operator=(Event&&) = delete;
	};
}
