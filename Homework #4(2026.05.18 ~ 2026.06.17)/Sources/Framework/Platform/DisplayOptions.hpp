#pragma once

#include <cstdint>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "../Math/Vector2D.hpp"

namespace TUK::Framework
{
	struct DisplayOptions final
	{
		LPCTSTR caption;
		Vector2D<std::int32_t> position;
		Vector2D<std::int32_t> size;
	};
}
