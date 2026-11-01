#pragma once

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"

#include "Event.hpp"

namespace TUK::Framework
{
	struct DisplayCreateEvent : Event
	{
	};

	struct DisplayCloseEvent : Event
	{
	};

	struct DisplayDestroyEvent : Event
	{
	};

	struct DisplayFocusEvent : Event
	{
		bool isFocused = false;
	};

	struct DisplayMoveEvent : Event
	{
		Vector2D<i32> position{ 0, 0};
	};

	struct DisplayResizeEvent : Event
	{
		Vector2D<i32> size{ 0 , 0 };
	};
}
