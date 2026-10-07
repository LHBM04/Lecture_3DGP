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

	const Vector2D<int>& Window::GetPosition() const noexcept
	{
		return options.position;
	}

	void Window::SetPosition(const Vector2D<int>& position) noexcept
	{
		if (SetWindowPos(hWnd, nullptr, position.GetX(), position.GetY(), 0, 0, SWP_NOSIZE | SWP_NOZORDER))
		{
			UpdateBounds();
		}
	}

	const Vector2D<int>& Window::GetSize() const noexcept
	{
		return options.size;
	}

	void Window::SetSize(const Vector2D<int>& size) noexcept
	{
		if (SetWindowPos(hWnd, nullptr, 0, 0, size.GetX(), size.GetY(), SWP_NOMOVE | SWP_NOZORDER))
		{
			UpdateBounds();
		}
	}

	void Window::UpdateBounds() noexcept
	{
		RECT rect{};
		if (GetWindowRect(hWnd, &rect))
		{
			options.position.Set(rect.left, rect.top);
			options.size.Set(rect.right - rect.left, rect.bottom - rect.top);
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
				UpdateBounds();
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
