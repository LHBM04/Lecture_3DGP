#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Application
{
	struct Options final
	{
		LPCTSTR title;
		int x;
		int y;
		int width;
		int height;
		DWORD style;
	};

	int Run(const Options& options);

	[[nodiscard]] HWND GetHWND() noexcept;

	[[nodiscard]] bool IsRunning();
	void SetRunning(bool running);
}