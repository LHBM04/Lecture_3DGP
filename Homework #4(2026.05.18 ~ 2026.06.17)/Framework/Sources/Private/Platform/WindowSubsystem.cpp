#include "Precompiled.h"
#include "Framework/Platform/WindowSubsystem.h"

#include "Framework/Core/Engine.h"

#include "Framework/Platform/Window.h"
#include "Framework/Platform/WindowFlags.h"
#include "Framework/Platform/WindowOptions.h"

#include "Platform/WindowInternal.h"

namespace
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
			default:
			{
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
		}
	}
}

namespace TUK::Framework
{
	void WindowSubsystem::OnStartup()
	{
		WNDCLASSEXW windowClass{};
		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = GetModuleHandle(nullptr);
		windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		windowClass.lpszClassName = L"TUK Framework";
		windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		RegisterClassExW(&windowClass);
	}

	void WindowSubsystem::OnShutdown()
	{
		UnregisterClassW(L"TUK Framework", GetModuleHandle(nullptr));
	}

	void WindowSubsystem::OnPreTick()
	{
		MSG msg{};
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				Engine::GetInstance().RequestQuit();
				return;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void WindowSubsystem::OnPostTick()
	{
		if (windows.empty())
		{
			Engine::GetInstance().RequestQuit();
		}
	}

	std::expected<std::reference_wrapper<Window>, std::error_code> WindowSubsystem::AddWindow(const WindowOptions& options)
	{
		HWND hWnd = CreateWindowExW(
			0,
			L"TUK Framework",
			options.title.data(),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			options.positionX, 
			options.positionY, 
			options.sizeX, 
			options.sizeY,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr), 
			nullptr
		);
		if (!hWnd)
		{
			return std::unexpected(std::make_error_code(std::errc::invalid_argument));
		}

		std::unique_ptr<Window>& window = windows.emplace_back(std::make_unique<WindowInternal>(hWnd));
		return std::ref(*window);
	}

	void WindowSubsystem::RemoveWindow(const Window& window)
	{
	}
}
