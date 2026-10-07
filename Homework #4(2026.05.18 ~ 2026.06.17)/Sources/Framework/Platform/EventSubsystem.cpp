#include "Precompiled.h"
#include "EventSubsystem.h"

#include "../Core/Engine.h"
#include "Window.h"

namespace
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto* window = reinterpret_cast<TUK::Framework::Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		if (message == WM_NCCREATE)
		{
			const auto* creation = reinterpret_cast<const CREATESTRUCTW*>(lParam);
			window = static_cast<TUK::Framework::Window*>(creation->lpCreateParams);
			if (!window)
			{
				return FALSE;
			}
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		}
		else if (message == WM_NCDESTROY)
		{
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
		}

		if (window)
		{
			return window->HandleMessage(hWnd, message, wParam, lParam);
		}
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
}

namespace TUK::Framework
{
	EventSubsystem::EventSubsystem() noexcept
		: EngineSubsystem(1)
	{
	}

	EventSubsystem::~EventSubsystem() noexcept = default;

	WNDPROC EventSubsystem::GetWindowProc() noexcept
	{
		return &WindowProc;
	}

	void EventSubsystem::OnPreTick()
	{
		auto& system = Engine::GetInstance();
		if (!system.IsRunning())
		{
			return;
		}

		MSG message{};
		while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				system.RequestQuit(static_cast<int>(message.wParam));
				break;
			}
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}
}
