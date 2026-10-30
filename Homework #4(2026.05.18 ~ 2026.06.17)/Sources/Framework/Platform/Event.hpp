#pragma once

#include <variant>

#include "DisplayEventData.hpp"
#include "InputEventData.hpp"

namespace TUK::Framework
{
	class Display;

	using EventData = std::variant<
		/** Display Events */
		DisplayCreateData,
		DisplayCloseData,
		DisplayDestroyData,
		DisplayFocusData,
		DisplayMoveData,
		DisplayResizeData,
		/** Input Events */
		KeyInteractData>;

	struct Event final
	{
		const Display& display;
		EventData data;
	};
}
