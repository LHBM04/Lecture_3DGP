#include "Precompiled.h"
#include "Window.h"

void Window::Show() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_SHOW);
	}
}

void Window::Hide() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_HIDE);
	}
}

HWND Window::GetHWND() const noexcept
{
	return hWnd;
}

const std::wstring& Window::GetTitle() const noexcept
{
	return options.title;
}

void Window::SetTitle(std::wstring title_) noexcept
{
	if (hWnd != nullptr)
	{
		options.title = std::move(title_);
		::SetWindowTextW(hWnd, options.title.c_str());
	}
}

int Window::GetWidth() const noexcept
{
	return options.width;
}

void Window::SetWidth(int width_) noexcept
{
	SetSize(SIZE{ width_, options.height });
}

int Window::GetHeight() const noexcept
{
	return options.height;
}

void Window::SetHeight(int height_) noexcept
{
	SetSize(SIZE{ options.width, height_ });
}

SIZE Window::GetSize() const noexcept
{
	return SIZE{ options.width, options.height };
}

void Window::SetSize(SIZE size_) noexcept
{
	assert(width_ > 0 || height > 0);

	options.width = size_.cx;
	options.height = size_.cy;

	if (hWnd != nullptr)
	{
		RECT rect{};
		rect.right = options.width;
		rect.bottom = options.height;

		DWORD style{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		if (options.isFullscreen) style = WS_POPUP;
		else if (!options.isBorderless) style = WS_POPUP | (options.isResizable ? WS_THICKFRAME : 0);
		else if (options.isResizable) style |= WS_THICKFRAME | WS_MAXIMIZEBOX;

		constexpr DWORD exStyle{ WS_EX_APPWINDOW };
		if (::AdjustWindowRectEx(&rect, style, FALSE, exStyle))
		{
			::SetWindowPos(
				hWnd,
				nullptr,
				0,
				0,
				rect.right - rect.left,
				rect.bottom - rect.top,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}

POINT Window::GetPosition() const noexcept
{
	if (hWnd == nullptr)
	{
		return POINT{};
	}

	RECT rect{};
	if (!::GetWindowRect(hWnd, &rect))
	{
		return POINT{};
	}

	return POINT{ rect.left, rect.top };
}

int Window::GetX() const noexcept
{
	return GetPosition().x;
}

void Window::SetX(int x_) noexcept
{
	SetPosition(POINT{ x_, GetY() });
}

int Window::GetY() const noexcept
{
	return GetPosition().y;
}

void Window::SetY(int y_) noexcept
{
	SetPosition(POINT{ GetX(), y_ });
}

void Window::SetPosition(POINT position_) noexcept
{
	assert(position_.x != 0 && position_.y != 0);

	if (hWnd != nullptr)
	{
		options.x = position_.x;
		options.y = position_.y;

		::SetWindowPos(
			hWnd, 
			nullptr, 
			options.x,
			options.y,
			0, 
			0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

bool Window::IsFullscreen() const noexcept
{
	return options.isFullscreen;
}

void Window::SetFullscreen(bool fullscreen_) noexcept
{
	if (hWnd != nullptr)
	{
		options.isFullscreen = fullscreen_;

		DWORD style{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		if (options.isFullscreen) style = WS_POPUP;
		else if (!options.isBorderless) style = WS_POPUP | (options.isResizable ? WS_THICKFRAME : 0);
		else if (options.isResizable) style |= WS_THICKFRAME | WS_MAXIMIZEBOX;

		constexpr DWORD exStyle{ WS_EX_APPWINDOW };
		::SetWindowLongPtrW(hWnd, GWL_STYLE, static_cast<LONG_PTR>(style));
		::SetWindowLongPtrW(hWnd, GWL_EXSTYLE, static_cast<LONG_PTR>(exStyle));
		::SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
}

bool Window::IsResizable() const noexcept
{
	return options.isResizable;
}

void Window::SetResizable(bool resizable_) noexcept
{
	if (hWnd != nullptr)
	{
		options.isResizable = resizable_;

		DWORD style{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		if (options.isFullscreen) style = WS_POPUP;
		else if (!options.isBorderless) style = WS_POPUP | (options.isResizable ? WS_THICKFRAME : 0);
		else if (options.isResizable) style |= WS_THICKFRAME | WS_MAXIMIZEBOX;

		constexpr DWORD exStyle{ WS_EX_APPWINDOW };
		::SetWindowLongPtrW(hWnd, GWL_STYLE, static_cast<LONG_PTR>(style));
		::SetWindowLongPtrW(hWnd, GWL_EXSTYLE, static_cast<LONG_PTR>(exStyle));
		::SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
}

bool Window::IsBorderless() const noexcept
{
	return options.isBorderless;
}

void Window::SetBorderless(bool decorated_) noexcept
{
	if (hWnd != nullptr)
	{
		options.isBorderless = decorated_;

		DWORD style{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		if (options.isFullscreen) style = WS_POPUP;
		else if (!options.isBorderless) style = WS_POPUP | (options.isResizable ? WS_THICKFRAME : 0);
		else if (options.isResizable) style |= WS_THICKFRAME | WS_MAXIMIZEBOX;

		constexpr DWORD exStyle{ WS_EX_APPWINDOW };
		::SetWindowLongPtrW(hWnd, GWL_STYLE, static_cast<LONG_PTR>(style));
		::SetWindowLongPtrW(hWnd, GWL_EXSTYLE, static_cast<LONG_PTR>(exStyle));
		::SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
}

bool Window::IsMinimized() const noexcept
{
	return hWnd != nullptr && ::IsIconic(hWnd);
}

void Window::Minimize() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_MINIMIZE);
	}
}

bool Window::IsMaximized() const noexcept
{
	return hWnd != nullptr && ::IsZoomed(hWnd);
}

void Window::Maximize() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_MAXIMIZE);
	}
}
