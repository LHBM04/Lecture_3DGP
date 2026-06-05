#include "Precompiled.h"
#include "Window.h"

HWND Window::GetHWND() const noexcept
{
	return hWnd;
}

LPCWSTR Window::GetTitle() const noexcept
{
	assert(hWnd != nullptr);

	LPWSTR title{ nullptr };
	::GetWindowTextW(hWnd, title, 0);
	return title;
}

void Window::SetTitle(LPCWSTR title_) noexcept
{
	assert(hWnd != nullptr);
	::SetWindowTextW(hWnd, title_);
}

UINT Window::GetWidth() const noexcept
{
	assert(hWnd != nullptr);

	RECT rect{};
	::GetClientRect(hWnd, &rect);
	return rect.right - rect.left;
}

void Window::SetWidth(UINT width_) noexcept
{
	assert(hWnd != nullptr);

	RECT rect{};
	::GetClientRect(hWnd, &rect);
	::SetWindowPos(hWnd, nullptr, 0, 0, width_, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

UINT Window::GetHeight() const noexcept
{
	assert(hWnd != nullptr);
	RECT rect{};
	::GetClientRect(hWnd, &rect);
	return rect.bottom - rect.top;
}

void Window::SetHeight(UINT height_) noexcept
{
	assert(hWnd != nullptr);
	RECT rect{};
	::GetClientRect(hWnd, &rect);
	::SetWindowPos(hWnd, nullptr, 0, 0, rect.right - rect.left, height_, SWP_NOMOVE | SWP_NOZORDER);
}

std::pair<UINT, UINT> Window::GetSize() const noexcept
{
	assert(hWnd != nullptr);
	RECT rect{};
	::GetClientRect(hWnd, &rect);
	return { rect.right - rect.left, rect.bottom - rect.top };
}

void Window::SetSize(UINT width_, UINT height_) noexcept
{
	assert(hWnd != nullptr);
	RECT rect{};
	::GetClientRect(hWnd, &rect);
	::SetWindowPos(hWnd, nullptr, 0, 0, width_, height_, SWP_NOMOVE | SWP_NOZORDER);
}

UINT Window::GetPositionX() const noexcept
{
	assert(hWnd != nullptr);
	POINT point{};
	::GetWindowRect(hWnd, reinterpret_cast<LPRECT>(&point));
	return point.x;
}

void Window::SetPositionX(UINT x_) noexcept
{
	assert(hWnd != nullptr);
	POINT point{};
	::GetWindowRect(hWnd, reinterpret_cast<LPRECT>(&point));
	::SetWindowPos(hWnd, nullptr, x_, point.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

UINT Window::GetPositionY() const noexcept
{
	assert(hWnd != nullptr);
	POINT point{};
	::GetWindowRect(hWnd, reinterpret_cast<LPRECT>(&point));
	return point.y;
}

void Window::SetPositionY(UINT y_) noexcept
{
	assert(hWnd != nullptr);
	POINT point{};
	::GetWindowRect(hWnd, reinterpret_cast<LPRECT>(&point));
	::SetWindowPos(hWnd, nullptr, point.x, y_, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

std::pair<UINT, UINT> Window::GetPosition() const noexcept
{
	assert(hWnd != nullptr);
	POINT point{};
	::GetWindowRect(hWnd, reinterpret_cast<LPRECT>(&point));
	return { point.x, point.y };
}
