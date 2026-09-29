#include "Precompiled.h"
#include "Platform/WindowInternal.h"

namespace TUK::Framework
{
	WindowInternal::WindowInternal(HWND hWnd) noexcept
		: hWnd(hWnd)
	{
	}

	WindowInternal::~WindowInternal() noexcept
	{
		if (hWnd)
		{
			DestroyWindow(hWnd);
			hWnd = nullptr;
		}
	}

	std::wstring WindowInternal::GetTitle() const
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		std::wstring title(256, L'\0');
		GetWindowText(hWnd, title.data(), title.size());
		return title;
	}

	void WindowInternal::SetTitle(std::wstring_view title)
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		SetWindowTextW(hWnd, title.data());
	}

	int WindowInternal::GetSizeX() const
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return rect.right - rect.left;
	}

	void WindowInternal::SetSizeX(int sizeX)
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		SetWindowPos(hWnd, nullptr, 0, 0, sizeX, GetSizeY(), SWP_NOMOVE | SWP_NOZORDER);
	}

	int WindowInternal::GetSizeY() const
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return rect.bottom - rect.top;
	}

	void WindowInternal::SetSizeY(int sizeY)
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		SetWindowPos(hWnd, nullptr, 0, 0, GetSizeX(), sizeY, SWP_NOMOVE | SWP_NOZORDER);
	}

	int WindowInternal::GetPositionX() const
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return rect.left;
	}

	void WindowInternal::SetPositionX(int positionX)
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		SetWindowPos(hWnd, nullptr, positionX, GetPositionY(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	int WindowInternal::GetPositionY() const
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return rect.top;
	}

	void WindowInternal::SetPositionY(int positionY)
	{
		ASSERT(hWnd, "hWnd가 nullptr입니다!");
		SetWindowPos(hWnd, nullptr, GetPositionX(), positionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
}
