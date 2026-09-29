#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Framework/Platform/Window.h"

namespace TUK::Framework
{
	class WindowInternal : public Window
	{
	public:
		explicit WindowInternal(HWND hWnd) noexcept;
		~WindowInternal() noexcept override;

		WindowInternal(const WindowInternal&) = delete;
		WindowInternal& operator=(const WindowInternal&) = delete;

		WindowInternal(WindowInternal&&) = delete;
		WindowInternal& operator=(WindowInternal&&) = delete;

		[[nodiscard]] std::wstring GetTitle() const noexcept override;
		void SetTitle(std::wstring_view title) noexcept override;

		[[nodiscard]] int GetSizeX() const noexcept override;
		void SetSizeX(int sizeX) noexcept override;

		[[nodiscard]] int GetSizeY() const noexcept override;
		void SetSizeY(int sizeY) noexcept override;

		[[nodiscard]] int GetPositionX() const noexcept override;
		void SetPositionX(int positionX) noexcept override;

		[[nodiscard]] int GetPositionY() const noexcept override;
		void SetPositionY(int positionY) noexcept override;

		[[nodiscard]] HWND GetHWND() const noexcept;

	private:
		HWND hWnd;
	};
}
