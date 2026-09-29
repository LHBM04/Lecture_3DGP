#pragma once

namespace TUK::Framework
{
	enum WindowFlags : unsigned int
	{
		None = 0,
		Resizable = 1 << 0,
		Fullscreen = 1 << 1,
		Borderless = 1 << 2,
	};
}
