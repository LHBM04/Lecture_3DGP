#pragma once

#include <string>

struct WindowOptions final
{
	std::wstring title;
	int x;
	int y;
	int width;
	int height;
	bool isFullscreen;
	bool isBorderless;
	bool isResizable;
};
