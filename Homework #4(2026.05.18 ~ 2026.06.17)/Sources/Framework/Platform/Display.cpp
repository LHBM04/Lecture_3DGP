#include "Precompiled.hpp"
#include "Display.hpp"

#include "../Core/Engine.hpp"

#include "DisplayCloseEvent.hpp"
#include "DisplayCreateEvent.hpp"
#include "DisplayDestroyEvent.hpp"
#include "DisplayFocusEvent.hpp"
#include "DisplayMoveEvent.hpp"
#include "DisplayResizeEvent.hpp"
#include "EventSubsystem.hpp"

namespace TUK::Framework
{
	Display::Display(HWND hWnd) noexcept
		: hWnd(hWnd)
		, shouldClose(false)
	{
	}

	Display::~Display() noexcept
	{
		if (hWnd)
		{
			DestroyWindow(hWnd);
			hWnd = nullptr;
		}
	}

#if defined(_UNICODE) || defined(UNICODE)
	const std::wstring& Display::GetCaption() const noexcept
	{
		assert(hWnd);
		static std::wstring caption;
		caption.resize(GetWindowTextLengthW(hWnd) + 1);
		GetWindowTextW(hWnd, caption.data(), static_cast<std::int32_t>(caption.size()));
		return caption;
	}

	void Display::SetCaption(std::wstring_view value) noexcept
	{
		assert(hWnd);
		SetWindowTextW(hWnd, value.data());
	}
#else
	const std::string& Display::GetCaption() const
	{
		assert(hWnd);
		static std::string caption;
		caption.resize(GetWindowTextLengthA(hWnd) + 1);
		GetWindowTextA(hWnd, caption.data(), static_cast<std::int32_t>(caption.size()));
		return caption;
	}

	void Display::SetCaption(std::string_view value) noexcept
	{
		assert(hWnd);
		SetWindowTextA(hWnd, value.data());
	}
#endif

	Vector2D<std::int32_t> Display::GetPosition() const noexcept
	{
		assert(hWnd);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return { rect.left, rect.top };
	}

	void Display::SetPosition(const Vector2D<std::int32_t>& position) noexcept
	{
		assert(hWnd);
		SetWindowPos(
			hWnd, 
			nullptr, 
			position.GetX(), 
			position.GetY(), 
			0, 
			0, 
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	}

	Vector2D<std::int32_t> Display::GetSize() const noexcept
	{
		assert(hWnd);
		RECT rect;
		GetWindowRect(hWnd, &rect);
		return { rect.right - rect.left, rect.bottom - rect.top };
	}

	void Display::SetSize(const Vector2D<std::int32_t>& size) noexcept
	{
		assert(hWnd);
		SetWindowPos(
			hWnd, 
			nullptr, 
			0, 
			0, 
			size.GetX(), 
			size.GetY(), 
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}

	Vector2D<std::int32_t> Display::GetClientSize() const noexcept
	{
		assert(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		return { rect.right - rect.left, rect.bottom - rect.top };
	}

	void Display::SetClientSize(const Vector2D<std::int32_t>& size) noexcept
	{
		assert(hWnd);

		RECT rect{ 0, 0, size.GetX(), size.GetY() };
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

	HWND Display::GetHWND() const noexcept
	{
		return hWnd;
	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const CREATESTRUCTW* creation = reinterpret_cast<const CREATESTRUCTW*>(lParam);

			Display* display = static_cast<Display*>(creation->lpCreateParams);
			if (!display)
			{
				return FALSE;
			}

			display->hWnd = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(display));

			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		Display* display = reinterpret_cast<Display*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (!display)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		EventSubsystem* eventSubsystem = Engine::GetInstance()->GetSubsystem<EventSubsystem>();

		switch (message)
		{
		case WM_CREATE:
		{
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayCreateEvent>(*display);
			}
			return 0;
		}
		case WM_SIZE:
		{
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayResizeEvent>(
					*display,
					Vector2D<std::int32_t>{ LOWORD(lParam), HIWORD(lParam) });
			}
			return 0;
		}
		case WM_MOVE:
		{
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayMoveEvent>(
					*display,
					Vector2D<std::int32_t>{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
			}
			return 0;
		}
		case WM_SETFOCUS: [[fallthrough]];
		case WM_KILLFOCUS:
		{
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayFocusEvent>(*display, message == WM_SETFOCUS);
			}
			return 0;
		}
		case WM_CLOSE:
		{
			display->SetShouldClose(true);
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayCloseEvent>(*display);
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		case WM_DESTROY:
		{
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayDestroyEvent>(*display);
			}
			return 0;
		}
		case WM_NCDESTROY:
		{
			SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
			display->hWnd = nullptr;
			return 0;
		}
		default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		}

		std::unreachable();
	}
}
