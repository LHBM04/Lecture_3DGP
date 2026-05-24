#pragma once

#include <Windows.h>

struct RendererOptions final
{
	HWND window;
	int x;
	int y;
	int width;
	int height;
	bool msaa4xEnable;
	bool enableTripleBuffering;
	bool vSync;
};
