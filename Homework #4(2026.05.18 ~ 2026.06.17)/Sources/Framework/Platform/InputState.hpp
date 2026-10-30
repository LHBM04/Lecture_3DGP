#pragma once

namespace TUK::Framework
{
	enum class InputState : std::uint8_t
	{
		None = 0,
		Pressed = 1 << 0,
		Released = 1 << 1,
		Repeat = 1 << 2
	};
}
