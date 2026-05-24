#pragma once

#include <Windows.h>

struct WindowOptions final
{
	std::wstring title;
	int x;
	int y;
	int width;
	int height;
	DWORD style;
	DWORD styleEx;
};
