#pragma once

#include <string>
#include <string_view>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "../Math/Vector2D.h"
#include "WindowOptions.h"

namespace TUK::Framework
{
	class Window
	{
	public:
		Window(const WindowOptions& options);
		~Window() noexcept;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		[[nodiscard]] HWND GetHWND() const noexcept;

		[[nodiscard]] const std::wstring& GetTitle() const noexcept;
		void SetTitle(std::wstring_view title) noexcept;

		[[nodiscard]] const Vector2D<int>& GetPosition() const noexcept;
		void SetPosition(const Vector2D<int>& position) noexcept;

		[[nodiscard]] const Vector2D<int>& GetSize() const noexcept;
		void SetSize(const Vector2D<int>& size) noexcept;

		[[nodiscard]] bool IsResizable() const noexcept;
		[[nodiscard]] bool IsBorderless() const noexcept;
		[[nodiscard]] bool IsFullscreen() const noexcept;

		[[nodiscard]] bool ShouldClose() const noexcept;
		void RequestClose() noexcept;

		LRESULT HandleMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

	private:
		void UpdateBounds() noexcept;

		HWND hWnd;
		WindowOptions options;
		bool shouldClose;
	};
}

