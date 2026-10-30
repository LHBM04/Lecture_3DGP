#pragma once

#include <cstdint>
#include <string>

#include "../Math/Vector2D.hpp"

namespace TUK::Framework
{
	struct DisplayCreateInfo final
	{
		std::string caption;
		Vector2D<std::int32_t> position;
		Vector2D<std::int32_t> size;
	};
}
