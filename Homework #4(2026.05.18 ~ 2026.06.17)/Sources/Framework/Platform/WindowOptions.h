#pragma once

#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

namespace TUK::Framework
{
	struct WindowOptions final
	{
		std::wstring title;

		int positionX;
		int positionY;

		int sizeX;
		int sizeY;

		bool isResizable;
		bool hasMinimizeButton;
		bool hasMaximizeButton;
		bool isAlwaysOnTop;
		bool isVisible;
	};
}
