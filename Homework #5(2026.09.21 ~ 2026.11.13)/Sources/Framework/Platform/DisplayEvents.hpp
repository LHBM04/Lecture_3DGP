#pragma once

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"

#include "Event.hpp"

namespace TUK::Framework
{
	class Display;

	struct DisplayEvent : Event
	{
		explicit DisplayEvent(const Display& display) noexcept;

		const Display& display;
	};

	struct DisplayCreateEvent : DisplayEvent
	{
		explicit DisplayCreateEvent(const Display& display) noexcept;
	};

	struct DisplayCloseEvent : DisplayEvent
	{
		explicit DisplayCloseEvent(const Display& display) noexcept;
	};

	struct DisplayDestroyEvent : DisplayEvent
	{
		explicit DisplayDestroyEvent(const Display& display) noexcept;
	};

	struct DisplayFocusEvent : DisplayEvent
	{
		DisplayFocusEvent(const Display& display, bool isFocused) noexcept;

		const bool isFocused;
	};

	struct DisplayMoveEvent : DisplayEvent
	{
		DisplayMoveEvent(const Display& display, const Vector2D<i32>& position) noexcept;

		const Vector2D<i32> position;
	};

	struct DisplayResizeEvent : DisplayEvent
	{
		DisplayResizeEvent(const Display& display, const Vector2D<i32>& size) noexcept;

		const Vector2D<i32> size;
	};

	inline DisplayEvent::DisplayEvent(const Display& display) noexcept
		: display(display)
	{
	}

	inline DisplayCreateEvent::DisplayCreateEvent(const Display& display) noexcept
		: DisplayEvent(display)
	{
	}

	inline DisplayCloseEvent::DisplayCloseEvent(const Display& display) noexcept
		: DisplayEvent(display)
	{
	}

	inline DisplayDestroyEvent::DisplayDestroyEvent(const Display& display) noexcept
		: DisplayEvent(display)
	{
	}

	inline DisplayFocusEvent::DisplayFocusEvent(const Display& display, bool isFocused) noexcept
		: DisplayEvent(display)
		, isFocused(isFocused)
	{
	}

	inline DisplayMoveEvent::DisplayMoveEvent(const Display& display, const Vector2D<i32>& position) noexcept
		: DisplayEvent(display)
		, position(position)
	{
	}

	inline DisplayResizeEvent::DisplayResizeEvent(const Display& display, const Vector2D<i32>& size) noexcept
		: DisplayEvent(display)
		, size(size)
	{
	}
}
