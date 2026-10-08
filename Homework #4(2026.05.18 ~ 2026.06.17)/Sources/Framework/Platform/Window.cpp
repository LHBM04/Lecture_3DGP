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
		std::wstring windowTitle(title);
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

	bool Window::IsResizable() const noexcept
	{
		return options.isResizable;
	}

	bool Window::IsBorderless() const noexcept
	{
		return options.isBorderless;
	}

	bool Window::IsFullscreen() const noexcept
	{
		return options.isFullscreen;
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
			case WM_NCCALCSIZE:
			{
				if (options.isBorderless || options.isFullscreen)
				{
					return 0;
				}
				return DefWindowProcW(handle, message, wParam, lParam);
			}
			case WM_GETMINMAXINFO:
			{
				if (options.isBorderless && !options.isFullscreen)
				{
					MONITORINFO monitor{};
					monitor.cbSize = sizeof(monitor);
					if (GetMonitorInfoW(MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST), &monitor))
					{
						auto& bounds = *reinterpret_cast<MINMAXINFO*>(lParam);
						bounds.ptMaxPosition = { monitor.rcWork.left - monitor.rcMonitor.left,
							monitor.rcWork.top - monitor.rcMonitor.top };
						bounds.ptMaxSize = { monitor.rcWork.right - monitor.rcWork.left,
							monitor.rcWork.bottom - monitor.rcWork.top };
						return 0;
					}
				}
				return DefWindowProcW(handle, message, wParam, lParam);
			}
			case WM_NCHITTEST:
			{
				if (options.isBorderless && !options.isFullscreen && options.isResizable && !IsZoomed(handle))
				{
					RECT rect{};
					if (GetWindowRect(handle, &rect))
					{
						const UINT dpi = GetDpiForWindow(handle);
						const int borderX = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi)
							+ GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
						const int borderY = GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi)
							+ GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
						const int x = GET_X_LPARAM(lParam);
						const int y = GET_Y_LPARAM(lParam);
						const bool isLeft = x < rect.left + borderX;
						const bool isRight = x >= rect.right - borderX;
						const bool isTop = y < rect.top + borderY;
						const bool isBottom = y >= rect.bottom - borderY;
						if (isTop)
						{
							return isLeft ? HTTOPLEFT : isRight ? HTTOPRIGHT : HTTOP;
						}
						if (isBottom)
						{
							return isLeft ? HTBOTTOMLEFT : isRight ? HTBOTTOMRIGHT : HTBOTTOM;
						}
						if (isLeft)
						{
							return HTLEFT;
						}
						if (isRight)
						{
							return HTRIGHT;
						}
						return HTCLIENT;
					}
				}
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
				return DefWindowProcW(handle, message, wParam, lParam);
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
