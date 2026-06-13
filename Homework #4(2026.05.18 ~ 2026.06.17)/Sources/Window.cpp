#include "Precompiled.h"
#include "Window.h"

#include "WindowService.h"

namespace
{
	[[nodiscard]] DWORD GetStyle(const Window::Options& options_)
	{
		DWORD style{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		if (options_.isFullscreen)
		{
			style = WS_POPUP;
		}
		else if (!options_.isBorderless)
		{
			style = WS_OVERLAPPEDWINDOW | (options_.isResizable ? WS_THICKFRAME | WS_MAXIMIZEBOX : 0);
		}
		else if (options_.isResizable)
		{
			style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
		}

		return style;
	}

	// 일단 얘가 하는 일이 별로 없긴 함. 일관성을 위해 작성.
	[[nodiscard]] DWORD GetStyleEx(const Window::Options& options_)
	{
		// TODO: 지금 당장은 Style Ex 건드리는 설정이 없음.
		// 나중에 필요하면 채워놓을 것.
		return WS_EX_OVERLAPPEDWINDOW;
	}
}

bool Window::Initialize(const Options& options_)
{
	assert(options_.x != 0 || options_.y != 0);
	assert(options_.width > 0 || options_.height > 0);

	options = options_;

	// TODO: 화면 중앙 위치 가져와서 x, y만큼 이동한 위치에 생성하도록.
	int x{ CW_USEDEFAULT };
	int y{ CW_USEDEFAULT };
	int width{ options.width };
	int height{ options.height };

	DWORD style{ GetStyle(options_) };
	DWORD styleEx{ GetStyleEx(options_) };

	if (options.isFullscreen)
	{
		const HMONITOR monitor{ ::MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY) };
		MONITORINFO monitorInfo{ .cbSize = sizeof(MONITORINFO) };
		if (!::GetMonitorInfoW(monitor, &monitorInfo))
		{
			return false;
		}

		x = monitorInfo.rcMonitor.left;
		y = monitorInfo.rcMonitor.top;
		width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
	}
	else
	{
		RECT rect{ };
		rect.right = options.width;
		rect.bottom = options.height;

		if (!::AdjustWindowRectEx(
			&rect, 
			style, 
			FALSE, 
			styleEx))
		{
			return false;
		}

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	// 이거 WindowService 내 필드 사용하게끔 사용하게 바꾸기.
	hWnd = ::CreateWindowExW(
		styleEx,
		L"Framework Window Class",
		options.title.c_str(),
		style,
		x,
		y,
		width,
		height,
		nullptr,
		nullptr,
		::GetModuleHandleW(nullptr),
		this);

	if (hWnd == nullptr)
	{
		return false;
	}

	if (!::IsWindowVisible(hWnd))
	{
		::ShowWindow(hWnd, SW_SHOW);
		::UpdateWindow(hWnd);
	}

	return true;
}

void Window::Release() noexcept
{
	if (hWnd != nullptr)
	{
		::DestroyWindow(hWnd);
		hWnd = nullptr;
	}
}

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

		DWORD style{ GetStyle(options) };
		DWORD styleEx{ GetStyleEx(options) };

		if (::AdjustWindowRectEx(
			&rect, 
			style, 
			FALSE, 
			styleEx))
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
	assert(position_.x != 0 || position_.y != 0);

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

		DWORD style{ GetStyle(options) };
		::SetWindowLongPtrW(
			hWnd, 
			GWL_STYLE, 
			static_cast<LONG_PTR>(style));

		DWORD styleEx{ GetStyleEx(options) };
		::SetWindowLongPtrW(
			hWnd, 
			GWL_EXSTYLE, 
			static_cast<LONG_PTR>(styleEx));
		
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

		DWORD style{ GetStyle(options) };
		::SetWindowLongPtrW(
			hWnd, 
			GWL_STYLE, 
			static_cast<LONG_PTR>(style));

		DWORD styleEx{ GetStyleEx(options) };
		::SetWindowLongPtrW(
			hWnd, 
			GWL_EXSTYLE, 
			static_cast<LONG_PTR>(styleEx));

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

		DWORD style{ GetStyle(options) };
		::SetWindowLongPtrW(
			hWnd, 
			GWL_STYLE, 
			static_cast<LONG_PTR>(style));

		DWORD styleEx{ GetStyleEx(options) };
		::SetWindowLongPtrW(
			hWnd, 
			GWL_EXSTYLE, 
			static_cast<LONG_PTR>(styleEx));

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
