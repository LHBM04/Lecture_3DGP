#pragma once

#include <cstdint>

#include "../Math/Vector2D.hpp"

#include "Event.hpp"

namespace TUK::Framework
{
	class Display;

	// 클래스 정의
	class DisplayEvent : public Event
	{
	public:
		~DisplayEvent() noexcept override = default;

		[[nodiscard]] const Display& GetDisplay() const noexcept;

		operator const Display&() const noexcept;

	protected:
		explicit DisplayEvent(const Display& display) noexcept;

	private:
		const Display& display;
	};

	class DisplayCreateEvent : public DisplayEvent
	{
	public:
		explicit DisplayCreateEvent(const Display& display) noexcept;
		~DisplayCreateEvent() noexcept override = default;
	};

	class DisplayCloseEvent : public DisplayEvent
	{
	public:
		explicit DisplayCloseEvent(const Display& display) noexcept;
		~DisplayCloseEvent() noexcept override = default;
	};

	class DisplayDestroyEvent : public DisplayEvent
	{
	public:
		explicit DisplayDestroyEvent(const Display& display) noexcept;
		~DisplayDestroyEvent() noexcept override = default;
	};

	class DisplayFocusEvent : public DisplayEvent
	{
	public:
		DisplayFocusEvent(const Display& display, bool isFocused) noexcept;
		~DisplayFocusEvent() noexcept override = default;

		[[nodiscard]] bool IsFocused() const noexcept;

	private:
		bool isFocused;
	};

	class DisplayMoveEvent : public DisplayEvent
	{
	public:
		DisplayMoveEvent(const Display& display, const Vector2D<std::int32_t>& position) noexcept;
		~DisplayMoveEvent() noexcept override = default;

		[[nodiscard]] const Vector2D<std::int32_t>& GetPosition() const noexcept;

	private:
		Vector2D<std::int32_t> position;
	};

	class DisplayResizeEvent : public DisplayEvent
	{
	public:
		DisplayResizeEvent(const Display& display, const Vector2D<std::int32_t>& size) noexcept;
		~DisplayResizeEvent() noexcept override = default;

		[[nodiscard]] const Vector2D<std::int32_t>& GetSize() const noexcept;

	private:
		Vector2D<std::int32_t> size;
	};

	// 멤버 함수 정의
	inline const Display& DisplayEvent::GetDisplay() const noexcept
	{
		return display;
	}

	inline DisplayEvent::operator const Display&() const noexcept
	{
		return display;
	}

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

	inline bool DisplayFocusEvent::IsFocused() const noexcept
	{
		return isFocused;
	}

	inline DisplayMoveEvent::DisplayMoveEvent(const Display& display, const Vector2D<std::int32_t>& position) noexcept
		: DisplayEvent(display)
		, position(position)
	{
	}

	inline const Vector2D<std::int32_t>& DisplayMoveEvent::GetPosition() const noexcept
	{
		return position;
	}

	inline DisplayResizeEvent::DisplayResizeEvent(const Display& display, const Vector2D<std::int32_t>& size) noexcept
		: DisplayEvent(display)
		, size(size)
	{
	}

	inline const Vector2D<std::int32_t>& DisplayResizeEvent::GetSize() const noexcept
	{
		return size;
	}
}
