#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"

namespace TUK::Framework
{
	struct DisplayOptions final
	{
		LPCTSTR caption;
		Vector2D<i32> position;
		Vector2D<i32> size;
		DWORD style;
		DWORD styleEx;
		int show;
	};
}
