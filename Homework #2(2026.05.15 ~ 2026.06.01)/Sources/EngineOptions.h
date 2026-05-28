#pragma once

#include <string>

struct EngineOptions final
{
	std::wstring title;
	int x;
	int y;
	int width;
	int height;
	bool fullscreen;
	bool resizable;
	bool borderless;
	float fixedTime;
};
