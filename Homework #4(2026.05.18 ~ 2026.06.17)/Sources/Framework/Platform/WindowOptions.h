#pragma once

#include <string>

#include "../Math/Vector2D.h"

namespace TUK::Framework
{
	struct WindowOptions final
	{
		WindowOptions();

		std::wstring title;

		Vector2D<int> position;
		Vector2D<int> size;

		bool isResizable;
		bool isBorderless;
		bool isFullscreen;
	};
}
