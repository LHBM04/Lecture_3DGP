#include "Precompiled.h"
#include "WindowOptions.h"

namespace TUK::Framework
{
	WindowOptions::WindowOptions()
		: title(L"Window")
		, position(CW_USEDEFAULT, CW_USEDEFAULT)
		, size(1280, 720)
		, isResizable(true)
		, isBorderless(false)
		, isFullscreen(false)
	{
	}
}
