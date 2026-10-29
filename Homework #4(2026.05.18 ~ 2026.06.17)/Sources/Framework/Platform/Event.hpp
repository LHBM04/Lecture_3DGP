#pragma once

#include <variant>

#include "DisplayEventData.hpp"

namespace TUK::Framework
{
	class Display;

	using EventData = std::variant<
		DisplayCreateData,
		DisplayCloseData,
		DisplayDestroyData,
		DisplayFocusData,
		DisplayMoveData,
		DisplayResizeData>;

	struct Event final
	{
		const Display& display;
		EventData data;
	};
}
