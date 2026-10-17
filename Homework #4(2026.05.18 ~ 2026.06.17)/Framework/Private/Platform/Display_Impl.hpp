#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "Framework/Platform/Display.hpp"

namespace TUK::Framework
{
	class Display_Impl : public Display
	{
		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		explicit Display_Impl(HWND hWnd) noexcept;
		~Display_Impl() noexcept override;

#if defined(_UNICODE) || defined(UNICODE)
		[[nodiscard]] const std::wstring& GetCaption() const noexcept override;
		void SetCaption(std::wstring_view value) noexcept override;
#else
		[[nodiscard]] const std::string& GetCaption() const noexcept override;
		void SetCaption(std::string_view value) noexcept override;
#endif

		[[nodiscard]] Vector2D<int> GetPosition() const noexcept override;
		void SetPosition(const Vector2D<int>& position) noexcept override;

		[[nodiscard]] Vector2D<int> GetSize() const noexcept override;
		void SetSize(const Vector2D<int>& size) noexcept override;

		[[nodiscard]] Vector2D<int> GetClientSize() const noexcept override;
		void SetClientSize(const Vector2D<int>& size) noexcept override;

		[[nodiscard]] bool ShouldClose() const noexcept override;
		void SetShouldClose(bool value) noexcept override;

		[[nodiscard]] HWND GetHWND() const noexcept;

	private:
		HWND hWnd;
		bool shouldClose;
	};
}
