#pragma once

#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#include "../Math/Vector2D.hpp"

namespace TUK::Framework
{
	struct WindowOptions final
	{
		LPCTSTR caption;
		Vector2D<int> position;
		Vector2D<int> size;
		DWORD style;
	};
}
