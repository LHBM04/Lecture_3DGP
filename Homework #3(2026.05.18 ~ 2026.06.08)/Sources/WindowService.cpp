#include "Precompiled.h"
#include "WindowService.h"

#include "Engine.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		return 0;
	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	std::unreachable();
}

HWND WindowService::AddWindow(const WindowOptions& options_)
{
    HINSTANCE instance{ GetEngine().GetOption(L"App.Instance", ::GetModuleHandleW(nullptr)) };

    WNDCLASSEXW wc{ sizeof(WNDCLASSEXW) };
    if (!::GetClassInfoExW(instance, L"Garage Engine", &wc))
    {
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = instance;
        wc.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
        wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = L"Garage Engine";

        if (!::RegisterClassExW(&wc))
        {
            return nullptr;
        }
    }

    const int screenWidth{ GetSystemMetrics(SM_CXSCREEN) };
    const int screenHeight{ GetSystemMetrics(SM_CYSCREEN) };
    const int windowPosX{ ((screenWidth - options_.width) / 2) + options_.x };
    const int windowPosY{ ((screenHeight - options_.height) / 2) + options_.y };

    constexpr DWORD WindowStyle{ WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX };

    HWND hwnd{ ::CreateWindowExW(
        0,
        L"Garage Engine",
        options_.title.c_str(),
        WindowStyle,
        windowPosX, windowPosY,
        options_.width, options_.height,
        nullptr,
        nullptr,
        instance,
        nullptr) };

    if (hwnd != nullptr)
    {
        ::ShowWindow(hwnd, SW_SHOW);
        ::UpdateWindow(hwnd);

        windows.insert(hwnd);
    }

    return hwnd;
}

void WindowService::RemoveWindow(HWND window_)
{
    if (windows.contains(window_))
    {
        ::DestroyWindow(window_);
        windows.erase(window_);
    }
}

bool WindowService::PollEvents()
{
    MSG msg;
    while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }

    return true;
}

void WindowService::OnAdd()
{
    HINSTANCE instance{ GetEngine().GetOption(L"App.Instance", ::GetModuleHandleW(nullptr)) };

    WNDCLASSEXW wndClass{};
    wndClass.cbSize = sizeof(WNDCLASSEXW);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = instance;
    wndClass.hIcon = ::LoadIconW(instance, IDI_APPLICATION);
    wndClass.hCursor = ::LoadCursorW(instance, IDC_ARROW);
    wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wndClass.lpszClassName = L"Garage Engine";
    ::RegisterClassExW(&wndClass);
}

void WindowService::OnRemove()
{
    for (HWND hwnd : windows)
    {
        ::DestroyWindow(hwnd);
    }
    windows.clear();
    mainWindow = nullptr;
}

HWND WindowService::GetMainWindow() const noexcept
{
    return mainWindow;
}

void WindowService::SetMainWindow(HWND window_) noexcept
{
    mainWindow = window_;
}
