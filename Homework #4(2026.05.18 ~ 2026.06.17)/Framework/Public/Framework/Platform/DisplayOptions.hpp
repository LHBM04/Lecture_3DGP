#pragma once

#include <cstdint>
#include <string>

#include "../Math/Vector2D.hpp"

namespace TUK::Framework
{
	struct DisplayOptions final
	{
#if defined(_UNICODE) || defined(UNICODE)
		std::wstring caption;
#else
		std::string caption;
#endif
		Vector2D<std::int32_t> position;
		Vector2D<std::int32_t> size;
	};
}
