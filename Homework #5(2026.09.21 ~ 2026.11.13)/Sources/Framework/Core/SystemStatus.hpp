#pragma once

#include "Common.hpp"

namespace TUK::Framework
{
	enum class SystemStatus : u8
	{
		SUCCESS = 0,
		FAILURE = 0 << 1,
		CONTINUE = 0 << 2,
	};
}
