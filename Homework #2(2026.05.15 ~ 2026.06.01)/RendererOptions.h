#pragma once

#include "ColorRGB.h"

struct RendererOptions final
{
	HWND window{ nullptr };
	int width{ 0 };
	int height{ 0 };
	ColorRGB clearColor;
	bool allowTearing = true;
	bool vSync = true;
};
