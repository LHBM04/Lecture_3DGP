#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Framework/Platform/Window.h"

namespace TUK::Framework
{
	class WindowInternal : public Window
	{
	public:
		explicit WindowInternal() noexcept = delete;
		explicit WindowInternal(HWND hWnd) noexcept;
		virtual ~WindowInternal() noexcept;

		[[nodiscard]] std::wstring GetTitle() const override;
		void SetTitle(std::wstring_view title) override;

		[[nodiscard]] int GetSizeX() const override;
		void SetSizeX(int sizeX) override;

		[[nodiscard]] int GetSizeY() const override;
		void SetSizeY(int sizeY) override;

		[[nodiscard]] int GetPositionX() const override;
		void SetPositionX(int positionX) override;

		[[nodiscard]] int GetPositionY() const override;
		void SetPositionY(int positionY) override;

	private:
		HWND hWnd;
	};
}
