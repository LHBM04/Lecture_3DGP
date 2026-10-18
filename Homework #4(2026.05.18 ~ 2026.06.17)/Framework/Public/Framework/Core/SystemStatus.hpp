#pragma once

#include <cstdint>

namespace TUK::Framework
{
	enum class SystemStatus : std::uint8_t
	{
		Success = 0,
		Failed = 1,
		Continue = 2,
	};
}
