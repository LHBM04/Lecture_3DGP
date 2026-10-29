#pragma once

#include <cstdint>

namespace TUK::Framework
{
	struct DisplayCreateData final
	{
	};

	struct DisplayCloseData final
	{
	};

	struct DisplayDestroyData final
	{
	};

	struct DisplayFocusData final
	{
		bool isFocused;
	};

	struct DisplayMoveData final
	{
		std::int32_t x;
		std::int32_t y;
	};

	struct DisplayResizeData final
	{
		std::int32_t width;
		std::int32_t height;
	};
}
