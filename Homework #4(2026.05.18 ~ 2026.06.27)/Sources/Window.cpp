#include "Precompiled.h"
#include "Window.h"

bool Window::Initialize(const WindowOptions& options_)
{
	options = options_;

 	hWnd = ::CreateWindowExW(
		options.styleEx,
		L"Homework #3 Class",
		options.title,
		options.style,
		options.x, options.y,
		options.width, options.height,
		nullptr, 
		nullptr, 
		options.hInstance, 
		this);

	if (hWnd == nullptr)
	{
		return false;
	}

	RECT rect{};
	if (::GetWindowRect(hWnd, &rect))
	{
		options.x = rect.left;
		options.y = rect.top;
		options.width = rect.right - rect.left;
		options.height = rect.bottom - rect.top;
	}

	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	return true;
}

void Window::Release()
{
	if (hWnd != nullptr)
	{
		::DestroyWindow(hWnd);
		hWnd = nullptr;
	}
}

void Window::Show(int show_) const
{
	assert(hWnd != nullptr);
	::ShowWindow(hWnd, show_);
	::UpdateWindow(hWnd);
}

void Window::Hide() const
{
	assert(hWnd != nullptr);
	::ShowWindow(hWnd, SW_HIDE);
	::UpdateWindow(hWnd);
}

HWND Window::GetHWND() const noexcept
{
	return hWnd;
}

LPCWSTR Window::GetTitle() const noexcept
{
	assert(hWnd != nullptr);
	return options.title;
}

void Window::SetTitle(LPCWSTR title_) noexcept
{
	assert(hWnd != nullptr);
	options.title = title_;
	::SetWindowTextW(hWnd, title_);
}

UINT Window::GetWidth() const noexcept
{
	assert(hWnd != nullptr);
	return static_cast<UINT>(options.width);
}

void Window::SetWidth(UINT width_) noexcept
{
	assert(hWnd != nullptr);
	options.width = static_cast<int>(width_);
	::SetWindowPos(hWnd, nullptr, 0, 0, options.width, options.height, SWP_NOMOVE | SWP_NOZORDER);
}

UINT Window::GetHeight() const noexcept
{
	assert(hWnd != nullptr);
	return static_cast<UINT>(options.height);
}

void Window::SetHeight(UINT height_) noexcept
{
	assert(hWnd != nullptr);
	options.height = static_cast<int>(height_);
	::SetWindowPos(hWnd, nullptr, 0, 0, options.width, options.height, SWP_NOMOVE | SWP_NOZORDER);
}

SIZE Window::GetSize() const noexcept
{
	assert(hWnd != nullptr);
	return SIZE{ .cx = options.width, .cy = options.height };
}

void Window::SetSize(SIZE size_) noexcept
{
	assert(hWnd != nullptr);
	options.width = size_.cx;
	options.height = size_.cy;
	::SetWindowPos(hWnd, nullptr, 0, 0, options.width, options.height, SWP_NOMOVE | SWP_NOZORDER);
}

UINT Window::GetPositionX() const noexcept
{
	assert(hWnd != nullptr);
	return static_cast<UINT>(options.x);
}

void Window::SetPositionX(UINT x_) noexcept
{
	assert(hWnd != nullptr);
	options.x = static_cast<int>(x_);
	::SetWindowPos(hWnd, nullptr, options.x, options.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

UINT Window::GetPositionY() const noexcept
{
	assert(hWnd != nullptr);
	return static_cast<UINT>(options.y);
}

void Window::SetPositionY(UINT y_) noexcept
{
	assert(hWnd != nullptr);
	options.y = static_cast<int>(y_);
	::SetWindowPos(hWnd, nullptr, options.x, options.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

POINT Window::GetPosition() const noexcept
{
	assert(hWnd != nullptr);
	return POINT{ .x = options.x, .y = options.y };
}

void Window::SetPosition(POINT position_) noexcept
{
	assert(hWnd != nullptr);
	options.x = position_.x;
	options.y = position_.y;
	::SetWindowPos(hWnd, nullptr, options.x, options.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

LRESULT Window::HandleMessage(HWND hWnd_, UINT message_, WPARAM wParam_, LPARAM lParam_) noexcept
{
	switch (message_)
	{
	case WM_MOVE:
	case WM_SIZE:
	{
		if (hWnd_ != nullptr)
		{
			RECT rect{};
			if (::GetWindowRect(hWnd_, &rect))
			{
				options.x = rect.left;
				options.y = rect.top;
				options.width = rect.right - rect.left;
				options.height = rect.bottom - rect.top;
			}
		}

		return 0;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		return 0;
	}
	default:
		return ::DefWindowProcW(hWnd_, message_, wParam_, lParam_);
	}
}
