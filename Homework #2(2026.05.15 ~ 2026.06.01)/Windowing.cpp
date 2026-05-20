#include "Precompiled.h"
#include "WindowSystem.h"
#include "Window.h"
#include "WindowOptions.h"
#include "WindowCloseEvent.h"
#include "WindowResizeEvent.h"
#include "EventQueue.h"

Window::Window(const WindowOptions& options_, HWND handle_) noexcept
	: options(options_)
	, handle(handle_)
{
}

Window::~Window() noexcept
{
	if (nullptr != handle)
	{
		::DestroyWindow(handle);
		handle = nullptr;
	}
}

HWND Window::GetHandle() const noexcept
{
	return handle;
}

std::wstring Window::GetTitle() const noexcept
{
	return options.title;
}

void Window::SetTitle(const std::wstring& title_) noexcept
{
	options.title = title_;
	::SetWindowTextW(handle, options.title.c_str());
}

bool WindowSystem::Initialize(HINSTANCE instance_) noexcept
{
	instance = instance_;

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowSystem::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIconW(instance, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(instance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIconW(instance, IDI_APPLICATION);

	return RegisterClassExW(&wc);
}

void WindowSystem::Release() noexcept
{
	UnregisterClassW(className, instance);
}

std::unique_ptr<Window> WindowSystem::Create(const WindowOptions& options_) noexcept
{
	DWORD style = 0;
	DWORD exStyle = WS_EX_APPWINDOW;

	if (options_.isFullscreen)
	{
		style = WS_POPUP;
	}
	else
	{
		if (options_.hasBorder)
		{
			style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
			if (options_.isResizable)
			{
				style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
			}
		}
		else
		{
			style = WS_POPUP;
		}
	}

	RECT rect = { 0, 0, options_.width, options_.height };
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if (options_.isFullscreen)
	{
		x = 0;
		y = 0;
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
	}

	HWND handle = CreateWindowExW(
		exStyle,
		className,
		options_.title.c_str(),
		style,
		x, y, width, height,
		nullptr, nullptr, instance, nullptr
	);

	if (handle == nullptr)
	{
		return nullptr;
	}

	ShowWindow(handle, SW_SHOW);
	UpdateWindow(handle);

	return std::unique_ptr<Window>(new Window(options_, handle));
}

bool WindowSystem::PollEvents(EventQueue& eventQueue_) noexcept
{
	eventQueue_.Clear();
	currentEventQueue = &eventQueue_;

	MSG message{};
	while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
	{
		if (message.message == WM_QUIT)
		{
			currentEventQueue = nullptr;
			return false;
		}

		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}

	currentEventQueue = nullptr;
	return true;
}

LRESULT WindowSystem::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
		case WM_SIZE:
		{
			if (currentEventQueue)
			{
				WindowResizeEvent e;
				e.width = LOWORD(lParam);
				e.height = HIWORD(lParam);
				currentEventQueue->Push(std::move(e));
			}
			break;
		}
		case WM_CLOSE:
		{
			if (currentEventQueue)
			{
				currentEventQueue->Push(WindowCloseEvent{});
			}
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
