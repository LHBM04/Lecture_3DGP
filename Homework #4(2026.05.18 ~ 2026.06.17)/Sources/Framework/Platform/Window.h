#ifndef HOMEWORK04_FRAMEWORK_PLATFORM_WINDOW
#define HOMEWORK04_FRAMEWORK_PLATFORM_WINDOW

#include <string>
#include <string_view>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

		[[nodiscard]] const std::wstring& GetTitle() const noexcept;
		void SetTitle(std::wstring_view title) noexcept;

		[[nodiscard]] int GetSizeX() const noexcept;
		void SetSizeX(int sizeX) noexcept;

		[[nodiscard]] int GetSizeY() const noexcept;
		void SetSizeY(int sizeY) noexcept;

		[[nodiscard]] int GetPositionX() const noexcept;
		void SetPositionX(int positionX) noexcept;

		[[nodiscard]] int GetPositionY() const noexcept;
		void SetPositionY(int positionY) noexcept;

		[[nodiscard]] HWND GetHWND() const noexcept;

		[[nodiscard]] bool ShouldClose() const noexcept;
		void RequestClose() noexcept;

		/** WindowProc에서 전달하는 네이티브 메시지 처리 */
		LRESULT HandleMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

	private:
		HWND hWnd;
		WindowOptions options;
		bool shouldClose;
	};
}

#endif
