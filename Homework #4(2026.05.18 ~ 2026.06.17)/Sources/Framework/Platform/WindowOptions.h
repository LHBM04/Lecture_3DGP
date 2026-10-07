#pragma once

#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

namespace TUK::Framework
{
	struct WindowOptions final
	{
		WindowOptions()
			: title(L"Window")
			, positionX(CW_USEDEFAULT)
			, positionY(CW_USEDEFAULT)
			, sizeX(1280)
			, sizeY(720)
			, isResizable(true)
			, hasMinimizeButton(true)
			, hasMaximizeButton(true)
			, isAlwaysOnTop(false)
			, isVisible(true)
		{
		}

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

