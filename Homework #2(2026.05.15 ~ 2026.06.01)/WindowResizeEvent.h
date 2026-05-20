#pragma once

#include "WindowEvent.h"

struct WindowResizeEvent final : public WindowEvent
{
	int width;
	int height;
};
