#pragma once

#include <string>

#include "WindowFlags.h"

namespace TUK::Framework
{
	struct WindowOptions final
	{
		std::wstring title;
		int sizeX;
		int sizeY;
		int positionX;
		int positionY;
		WindowFlags flags;
	};
}
