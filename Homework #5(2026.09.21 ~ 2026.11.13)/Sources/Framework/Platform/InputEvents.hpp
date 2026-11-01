#pragma once

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"

#include "Event.hpp"
#include "InputStates.hpp"

namespace TUK::Framework
{
	struct KeyInterectEvent : Event
	{
		KeyCode key = KeyCode::NONE;
		InputAction action = InputAction::PRESSED;
	};

	struct ButtonInterectEvent : Event
	{
		ButtonCode button = ButtonCode::NONE;
		InputAction action = InputAction::PRESSED;
	};

	struct MouseMoveEvent : Event
	{
		Vector2D<i32> position{};
	};

	struct MouseWheelEvent : Event
	{
		Vector2D<i32> wheelDelta{};
	};

	struct MouseHoverEvent : Event
	{
	};

	struct MouseRawInputEvent : Event
	{
		Vector2D<i32> delta{};
	};

	struct InputActivationEvent : Event
	{
		bool isActive = false;
	};
}
