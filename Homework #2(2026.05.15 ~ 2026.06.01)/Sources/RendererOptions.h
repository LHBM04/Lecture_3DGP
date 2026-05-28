#pragma once

#include <windows.h>

struct RendererOptions final
{
	HWND window{ nullptr };
	int width{ 0 };
	int height{ 0 };
	bool vSync{ true };
	bool fullscreen{ false };
	bool enableTripleBuffering{ false };
};
