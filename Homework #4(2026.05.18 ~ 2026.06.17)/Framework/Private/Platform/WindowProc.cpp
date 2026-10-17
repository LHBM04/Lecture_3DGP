#include "Precompiled.hpp"
#include "WindowProc.hpp"

#include "Framework/Core/Engine.hpp"

#include "Framework/Platform/DisplayCloseEvent.hpp"
#include "Framework/Platform/DisplayCreateEvent.hpp"
#include "Framework/Platform/DisplayDestroyEvent.hpp"
#include "Framework/Platform/DisplayFocusEvent.hpp"
#include "Framework/Platform/DisplayMoveEvent.hpp"
#include "Framework/Platform/DisplayResizeEvent.hpp"
#include "Framework/Platform/EventSubsystem.hpp"

#include "Display_Impl.hpp"

namespace TUK::Framework
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto* display = reinterpret_cast<Display_Impl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (message == WM_NCCREATE)
		{
			const auto* creation = reinterpret_cast<const CREATESTRUCTW*>(lParam);
			display = static_cast<Display_Impl*>(creation->lpCreateParams);
			if (!display)
			{
				return FALSE;
			}

			display->hWnd = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(display));
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		if (!display)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		auto* eventSubsystem = Engine::GetInstance()->GetSubsystem<EventSubsystem>();

		switch (message)
		{
		case WM_CREATE:
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayCreateEvent>(*display);
			}
			return 0;

		case WM_CLOSE:
			display->SetShouldClose(true);
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayCloseEvent>(*display);
			}
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_DESTROY:
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayDestroyEvent>(*display);
			}
			return 0;

		case WM_NCDESTROY:
			SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
			display->hWnd = nullptr;
			return 0;

		case WM_SIZE:
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayResizeEvent>(
					*display,
					Vector2D<int>{ static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)) });
			}
			return 0;

		case WM_MOVE:
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayMoveEvent>(
					*display,
					Vector2D<int>{ static_cast<int>(GET_X_LPARAM(lParam)), static_cast<int>(GET_Y_LPARAM(lParam)) });
			}
			return 0;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			if (eventSubsystem)
			{
				eventSubsystem->PushEvent<DisplayFocusEvent>(*display, message == WM_SETFOCUS);
			}
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		std::unreachable();
	}
}
