#include "Precompiled.hpp"
#include "Display_Impl.hpp"

namespace TUK::Framework
{
	Display_Impl::Display_Impl(HWND hWnd) noexcept
		: hWnd(hWnd)
		, shouldClose(false)
	{
		assert(this->hWnd);
	}

	Display_Impl::~Display_Impl() noexcept
	{
		if (hWnd)
		{
			DestroyWindow(hWnd);
			hWnd = nullptr;
		}
	}

#if defined(_UNICODE) || defined(UNICODE)
	const std::wstring& Display_Impl::GetCaption() const noexcept
	{
		assert(hWnd);
		static std::wstring caption;
		caption.resize(GetWindowTextLengthW(hWnd) + 1);
		GetWindowTextW(hWnd, caption.data(), static_cast<int>(caption.size()));
		return caption;
	}

	void Display_Impl::SetCaption(std::wstring_view value) noexcept
	{
		assert(hWnd);
		SetWindowTextW(hWnd, value.data());
	}
#else
	const std::string& Window_Impl::GetCaption() const
	{
		assert(hWnd);
		static std::string caption;
		caption.resize(GetWindowTextLengthA(hWnd) + 1);
		GetWindowTextA(hWnd, caption.data(), static_cast<int>(caption.size()));
		return caption;
	}

	void Window_Impl::SetCaption(std::string_view value) noexcept
	{
		assert(hWnd);
		SetWindowTextA(hWnd, value.data());
	}
#endif

	Vector2D<int> Display_Impl::GetPosition() const noexcept
	{
		assert(hWnd);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return { rect.left, rect.top };
	}

	void Display_Impl::SetPosition(const Vector2D<int>& position) noexcept
	{
		assert(hWnd);
		SetWindowPos(hWnd, nullptr, position.GetX(), position.GetY(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	Vector2D<int> Display_Impl::GetSize() const noexcept
	{
		assert(hWnd);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return { rect.right - rect.left, rect.bottom - rect.top };
	}

	void Display_Impl::SetSize(const Vector2D<int>& size) noexcept
	{
		assert(hWnd);
		SetWindowPos(hWnd, nullptr, 0, 0, size.GetX(), size.GetY(), SWP_NOMOVE | SWP_NOZORDER);
	}

	Vector2D<int> Display_Impl::GetClientSize() const noexcept
	{
		assert(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		return { rect.right - rect.left, rect.bottom - rect.top };
	}

	void Display_Impl::SetClientSize(const Vector2D<int>& size) noexcept
	{
		assert(hWnd);
		RECT rect{ 0, 0, size.GetX(), size.GetY() };
		const DWORD style = static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_STYLE));
		const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_EXSTYLE));
		AdjustWindowRectEx(&rect, style, GetMenu(hWnd) != nullptr, exStyle);
		SetWindowPos(hWnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	}

	bool Display_Impl::ShouldClose() const noexcept
	{
		return shouldClose;
	}

	void Display_Impl::SetShouldClose(bool value) noexcept
	{
		shouldClose = value;
	}

	HWND Display_Impl::GetHWND() const noexcept
	{
		assert(hWnd);
		return hWnd;
	}
}
