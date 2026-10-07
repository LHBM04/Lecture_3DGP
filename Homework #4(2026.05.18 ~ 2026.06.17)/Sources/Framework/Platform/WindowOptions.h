#pragma once

#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../Math/Vector2D.h"

namespace TUK::Framework
{
	struct WindowOptions final
	{
		std::wstring title;

		Vector2D<int> position;
		Vector2D<int> size;

		bool isResizable;
		bool hasMinimizeButton;
		bool hasMaximizeButton;
		bool isAlwaysOnTop;
		bool isVisible;
	};
}
