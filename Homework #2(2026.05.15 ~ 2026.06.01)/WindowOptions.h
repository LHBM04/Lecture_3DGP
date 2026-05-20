#pragma once

#include <string>

struct WindowOptions final
{
	std::wstring title = L"New Application";
	int width = 800;
	int height = 600;
	bool hasBorder = true;
	bool isResizable = true;
	bool isFullscreen = false;
};
