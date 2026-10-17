#pragma once

namespace TUK::Framework
{
	struct Event
	{
		virtual ~Event() noexcept;

	protected:
		Event() noexcept;
	};
}
