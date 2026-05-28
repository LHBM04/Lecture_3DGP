#include "Precompiled.h"
#include "Window.h"

#include "EventQueue.h"
#include "WindowCloseEvent.h"
#include "WindowMaximizeEvent.h"
#include "WindowMinimizeEvent.h"
#include "WindowMoveEvent.h"
#include "WindowResizeEvent.h"

std::expected<void, std::wstring> Window::Initialize(const WindowOptions& options_)
{
	if (nullptr != handle)
	{
		return std::unexpected{ L"Initialized already!!" };
	}

	options = options_;
	options.width = std::max(1, options.width);
	options.height = std::max(1, options.height);

	RECT rect{};
	rect.left = 0;
	rect.top = 0;
	rect.right = options.width;
	rect.bottom = options.height;
	::AdjustWindowRectEx(&rect, options.style, FALSE, options.styleEx);

	const int windowWidth{ rect.right - rect.left };
	const int windowHeight{ rect.bottom - rect.top };

	const int screenWidth{ ::GetSystemMetrics(SM_CXSCREEN) };
	const int screenHeight{ ::GetSystemMetrics(SM_CYSCREEN) };

	const int windowX{ (screenWidth - windowWidth) / 2 + options.x };
	const int windowY{ (screenHeight - windowHeight) / 2 + options.y };

	handle = ::CreateWindowExW(
		options.styleEx,
		L"Engine",
		options.title.c_str(),
		options.style,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		::GetModuleHandleW(nullptr),
		this);

	if (handle == nullptr)
	{
		// ::GetErr
		return std::unexpected{ L"" };
	}
}

void Window::Release()
{
	if (nullptr != handle)
	{
		::DestroyWindow(handle);
		handle = nullptr;
	}
}

HWND Window::GetHandle() const noexcept
{
	return handle;
}

const std::wstring& Window::GetTitle() const noexcept
{
	return options.title;
}

void Window::SetTitle(const std::wstring& title_) noexcept
{
	if (nullptr != handle)
	{
		options.title = title_;
		::SetWindowTextW(handle, options.title.c_str());
	}
}

int Window::GetPositionX() const noexcept
{
	return options.x;
}

void Window::SetPositionX(int x_) noexcept
{
	if (nullptr != handle)
	{
		options.x = x_;
		::MoveWindow(handle, options.x, options.y, options.width, options.height, FALSE);
	}
}

int Window::GetPositionY() const noexcept
{
	return options.y;
}

void Window::SetPositionY(int y_) noexcept
{
	if (nullptr != handle)
	{
		options.y = y_;
		::MoveWindow(handle, options.x, options.y, options.width, options.height, FALSE);
	}
}

std::pair<int, int> Window::GetPosition() const noexcept
{
	return { options.x, options.y };
}

void Window::SetPosition(int x_, int y_) noexcept
{
	if (nullptr != handle)
	{
		options.x = x_;
		options.y = y_;
		::MoveWindow(handle, options.x, options.y, options.width, options.height, FALSE);
	}
}

int Window::GetWidth() const noexcept
{
	return options.width;
}

void Window::SetWidth(int width_) noexcept
{
	options.width = std::max(1, width_);
}

int Window::GetHeight() const noexcept
{
	return options.height;
}

void Window::SetHeight(int height_) noexcept
{
	options.height = std::max(1, height_);
}

std::pair<int, int> Window::GetSize() const noexcept
{
	return { options.width, options.height };
}

void Window::SetSize(int width_, int height_)
{
	if (nullptr != handle)
	{
		options.width = std::max(1, width_);
		options.height = std::max(1, height_);
		::MoveWindow(handle, 0, 0, options.width, options.height, FALSE);
	}
}

DWORD Window::GetStyle() const noexcept
{
	return options.style;
}

void Window::SetStyle(DWORD style_) noexcept
{
	if (handle != nullptr)
	{
		options.style = style_;
		::SetWindowLongPtrW(handle, GWL_STYLE, style_);
		::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}

DWORD Window::GetStyleEx() const noexcept
{
	return options.styleEx;
}

void Window::SetStyleEx(DWORD styleEx_) noexcept
{
	if (handle != nullptr)
	{
		options.styleEx = styleEx_;
		::SetWindowLongPtrW(handle, GWL_EXSTYLE, styleEx_);
		::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}

void Window::Show() const noexcept
{
	if (nullptr != handle)
	{
		::ShowWindow(handle, SW_SHOWDEFAULT);
		::UpdateWindow(handle);
	}
}

void Window::Hide() const noexcept
{
	if (nullptr != handle)
	{
		::ShowWindow(handle, SW_HIDE);
		::UpdateWindow(handle);
	}
}

LRESULT Window::ProceedEvent(UINT message_, WPARAM wParam_, LPARAM lParam_)
{
	switch (message_)
	{
	case WM_CLOSE:
	{
		eventQueue.Push<WindowCloseEvent>(*this);

		return 0;
	}
	case WM_SIZE:
	{
		const UINT sizeType{ static_cast<UINT>(wParam_) };
		const int width{ LOWORD(lParam_) };
		const int height{ HIWORD(lParam_) };

		options.width = width;
		options.height = height;

		eventQueue.Push<WindowResizeEvent>(*this, width, height);

		if (sizeType == SIZE_MINIMIZED)
		{
			eventQueue.Push<WindowMinimizeEvent>(*this);
		}
		else if (sizeType == SIZE_MAXIMIZED)
		{
			eventQueue.Push<WindowMaximizeEvent>(*this);
		}

		return 0;
	}
	case WM_MOVE:
	{
		const int x{ static_cast<int>(static_cast<short>(LOWORD(lParam_))) };
		const int y{ static_cast<int>(static_cast<short>(HIWORD(lParam_))) };

		options.x = x;
		options.y = y;

		eventQueue.Push<WindowMoveEvent>(*this, x, y);

		return 0;
	}
	default:
		::DefWindowProcW(handle, message_, wParam_, lParam_);
	}

	std::unreachable();
}