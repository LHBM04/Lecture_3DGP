#pragma once

#include <cstdint>

namespace TUK::Framework
{
	enum class SystemStatus : std::uint8_t
	{
		Success = 0,
		Failure = 1,
		Continue = 2,
	};
}
