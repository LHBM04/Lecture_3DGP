#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <array>

#include <windows.h>

#include "DisplayOptions.hpp"

namespace TUK::Framework
{
	class EventSubsystem;
	class DisplaySubsystem;

	class Display
	{
		friend class EventSubsystem;
		friend class DisplaySubsystem;
		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		explicit Display(HWND hWnd) noexcept;
		~Display() noexcept;

		Display(const Display&) = delete;
		Display& operator=(const Display&) = delete;

		Display(Display&&) = default;
		Display& operator=(Display&&) = default;

		/** 반환 포인터는 다음 GetCaption 호출 또는 객체 이동 및 소멸 전까지 유효하다. */
		[[nodiscard]] LPCTSTR GetCaption() const noexcept;
		void SetCaption(LPCTSTR value) noexcept;

		[[nodiscard]] Vector2D<std::int32_t> GetPosition() const noexcept;
		void SetPosition(const Vector2D<std::int32_t>& position) noexcept;

		[[nodiscard]] Vector2D<std::int32_t> GetSize() const noexcept;
		void SetSize(const Vector2D<std::int32_t>& size) noexcept;

		[[nodiscard]] Vector2D<std::int32_t> GetClientSize() const noexcept;
		void SetClientSize(const Vector2D<std::int32_t>& size) noexcept;

		[[nodiscard]] bool ShouldClose() const noexcept;
		void SetShouldClose(bool value) noexcept;

		[[nodiscard]] HWND GetHWND() const noexcept;

	private:
		HWND hWnd;
		bool shouldClose;
	};

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}
