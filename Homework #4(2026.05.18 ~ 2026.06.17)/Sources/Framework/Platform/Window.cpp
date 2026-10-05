#include "Precompiled.h"
#include "Window.h"

namespace TUK::Framework
{
	Window::Window(const WindowOptions& options)
		: hWnd(nullptr)
		, options(options)
		, shouldClose(false)
	{
	}

	Window::~Window() noexcept
	{
		if (hWnd)
		{
			DestroyWindow(hWnd);
			hWnd = nullptr;
		}
	}

	const std::wstring& Window::GetTitle() const noexcept
	{
		return options.title;
	}

	void Window::SetTitle(std::wstring_view title) noexcept
	{
		std::wstring windowTitle{ title };
		if (SetWindowTextW(hWnd, windowTitle.c_str()))
		{
			options.title = std::move(windowTitle);
		}
	}

	int Window::GetSizeX() const noexcept
	{
		return options.sizeX;
	}

	void Window::SetSizeX(int sizeX) noexcept
	{
		if (SetWindowPos(hWnd, nullptr, 0, 0, sizeX, options.sizeY, SWP_NOMOVE | SWP_NOZORDER))
		{
			RECT rect{};
			if (GetWindowRect(hWnd, &rect))
			{
				options.positionX = rect.left;
				options.positionY = rect.top;
				options.sizeX = rect.right - rect.left;
				options.sizeY = rect.bottom - rect.top;
			}
		}
	}

	int Window::GetSizeY() const noexcept
	{
		return options.sizeY;
	}

	void Window::SetSizeY(int sizeY) noexcept
	{
		if (SetWindowPos(hWnd, nullptr, 0, 0, options.sizeX, sizeY, SWP_NOMOVE | SWP_NOZORDER))
		{
			RECT rect{};
			if (GetWindowRect(hWnd, &rect))
			{
				options.positionX = rect.left;
				options.positionY = rect.top;
				options.sizeX = rect.right - rect.left;
				options.sizeY = rect.bottom - rect.top;
			}
		}
	}

	int Window::GetPositionX() const noexcept
	{
		return options.positionX;
	}

	void Window::SetPositionX(int positionX) noexcept
	{
		if (SetWindowPos(hWnd, nullptr, positionX, options.positionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER))
		{
			RECT rect{};
			if (GetWindowRect(hWnd, &rect))
			{
				options.positionX = rect.left;
				options.positionY = rect.top;
				options.sizeX = rect.right - rect.left;
				options.sizeY = rect.bottom - rect.top;
			}
		}
	}

	int Window::GetPositionY() const noexcept
	{
		return options.positionY;
	}

	void Window::SetPositionY(int positionY) noexcept
	{
		if (SetWindowPos(hWnd, nullptr, options.positionX, positionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER))
		{
			RECT rect{};
			if (GetWindowRect(hWnd, &rect))
			{
				options.positionX = rect.left;
				options.positionY = rect.top;
				options.sizeX = rect.right - rect.left;
				options.sizeY = rect.bottom - rect.top;
			}
		}
	}

	HWND Window::GetHWND() const noexcept
	{
		return hWnd;
	}

	bool Window::ShouldClose() const noexcept
	{
		return shouldClose;
	}

	void Window::RequestClose() noexcept
	{
		shouldClose = true;
	}

	LRESULT Window::HandleMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (message)
		{
			case WM_NCCREATE:
			{
				hWnd = handle;
				return DefWindowProcW(handle, message, wParam, lParam);
			}
			case WM_CLOSE:
			{
				RequestClose();
				return 0;
			}
			case WM_CREATE: [[fallthrough]];
			case WM_WINDOWPOSCHANGED:
			{
				RECT rect{};
				if (GetWindowRect(hWnd, &rect))
				{
					options.positionX = rect.left;
					options.positionY = rect.top;
					options.sizeX = rect.right - rect.left;
					options.sizeY = rect.bottom - rect.top;
				}
				return 0;
			}
			case WM_SHOWWINDOW:
			{
				options.isVisible = wParam != 0;
				return 0;
			}
			case WM_NCDESTROY:
			{
				hWnd = nullptr;
				RequestClose();
				return 0;
			}
			default:
			{
				return DefWindowProcW(handle, message, wParam, lParam);
			}
		}

		std::unreachable();
	}
}
