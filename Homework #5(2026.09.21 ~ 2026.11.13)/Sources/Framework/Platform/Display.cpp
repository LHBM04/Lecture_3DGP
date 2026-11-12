#include "Precompiled.hpp"
#include "Display.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	Display::Display(HWND hWnd) noexcept
		: hWnd(hWnd)
		, shouldClose(false)
		, id(static_cast<ID>(static_cast<u32>(nextId) + 1))
	{
		nextId = id;
	}

	Display::~Display() noexcept
	{
		if (hWnd)
		{
			DestroyWindow(hWnd);
			hWnd = nullptr;
		}
	}

	HWND Display::GetHWND() const noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		return hWnd;
	}

	Display::ID Display::GetID() const noexcept
	{
		return id;
	}

	LPCTSTR Display::GetCaption() const noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		static std::array<TCHAR, 256> caption;
		std::ranges::fill(caption, TCHAR{});

		GetWindowText(hWnd, caption.data(), static_cast<int>(caption.size()));
		return caption.data();
	}

	void Display::SetCaption(LPCTSTR value) noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		SetWindowText(hWnd, value);
	}

	Vector2D<i32> Display::GetPosition() const noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		RECT rect;
		GetWindowRect(hWnd, &rect);
		
		return { rect.left, rect.top };
	}

	void Display::SetPosition(const Vector2D<i32>& position) noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		SetWindowPos(
			hWnd,
			nullptr,
			position.GetX(),
			position.GetY(),
			0,
			0,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	}

	Vector2D<i32> Display::GetSize() const noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		RECT rect;
		GetWindowRect(hWnd, &rect);
		
		return { rect.right - rect.left, rect.bottom - rect.top };
	}

	void Display::SetSize(const Vector2D<i32>& size) noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			size.GetX(),
			size.GetY(),
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}

	Vector2D<i32> Display::GetClientSize() const noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		RECT rect;
		GetClientRect(hWnd, &rect);
		
		return { rect.right - rect.left, rect.bottom - rect.top };
	}

	void Display::SetClientSize(const Vector2D<i32>& size) noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		RECT rect{};
		rect.right = size.GetX();
		rect.bottom = size.GetY();

		const DWORD style = static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_STYLE));
		const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_EXSTYLE));

		AdjustWindowRectEx(&rect, style, FALSE, exStyle);

		SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}

	bool Display::ShouldClose() const noexcept
	{
		return shouldClose;
	}

	void Display::SetShouldClose(bool value) noexcept
	{
		shouldClose = value;
	}

	DWORD Display::GetStyle() const noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		return static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_STYLE));
	}

	void Display::SetStyle(DWORD value) noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		SetWindowLongPtr(hWnd, GWL_STYLE, value);
	}

	DWORD Display::GetStyleEx() const noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		return static_cast<DWORD>(GetWindowLongPtr(hWnd, GWL_EXSTYLE));
	}

	void Display::SetStyleEx(DWORD value) noexcept
	{
		ASSERT_FMT(hWnd, "해당 Display 내 HWND가 nullptr입니다.");

		SetWindowLongPtr(hWnd, GWL_EXSTYLE, value);
	}
}
