#include "Precompiled.hpp"
#include "Window.hpp"

#include "Logger.hpp"
#include "Input.hpp"
#include "Renderer.hpp"

HWND Window::GetHWND()
{
	return hWnd;
}

bool Window::ShouldClose()
{
	return shouldClose;
}

void Window::SetShouldClose(bool shouldClose_)
{
	shouldClose = shouldClose_;
	if (shouldClose)
	{
		PostQuitMessage(0);
	}
}

LPCWSTR Window::GetTitle()
{
	return options.title;
}

void Window::SetTitle(LPCWSTR title_)
{
	SetWindowTextW(hWnd, title_);
}

DWORD Window::GetWindowStyle()
{
	DWORD style{ };

	if (options.isFullscreen)
	{
		style = WS_POPUP;
	}
	else if (options.isBorderless)
	{
		style = WS_POPUP;
	}
	else
	{
		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

		if (options.isResizable)
		{
			style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
		}
		else
		{
			style |= WS_MINIMIZEBOX;
		}
	}

	return style;
}

DWORD Window::GetWindowExStyle()
{
	DWORD exStyle{ WS_EX_APPWINDOW };

	if (options.isBorderless || options.isFullscreen)
	{
		exStyle |= WS_EX_TOPMOST;
	}

	return exStyle;
}

bool Window::Initialize(const Options& options_)
{
	options = options_;

	WNDCLASSEXW wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEXW));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Window::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = options.hInstance;
	wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = options.className;
	wc.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
	
	if (!RegisterClassExW(&wc))
	{
		Logger::Critical("윈도우 클래스 등록에 실패했습니다! 오류 코드: %d", GetLastError());
		return false;
	}

	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = static_cast<LONG>(options.width);
	windowRect.bottom = static_cast<LONG>(options.height);

	DWORD style = Window::GetWindowStyle();
	DWORD exStyle = Window::GetWindowExStyle();
	AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

	hWnd = CreateWindowExW(
		exStyle, options.className, options.title, style,
		static_cast<int>(options.x), static_cast<int>(options.y),
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, options.hInstance, nullptr
	);

	if (nullptr == hWnd)
	{
		Logger::Critical("윈도우 생성에 실패했습니다! 오류 코드: %d", GetLastError());
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return true;
}

void Window::PollEvent()
{
	MSG msg;
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT) 
		{
			shouldClose = true;
			break;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void Window::Terminate()
{
	if (hWnd)
	{
		DestroyWindow(hWnd);
		hWnd = nullptr;
	}

	UnregisterClassW(options.className, options.hInstance);
}

LRESULT CALLBACK Window::WndProc(HWND hWnd_, UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	switch (uMsg_)
	{
	case WM_KEYUP:
	{
		Input::SetKeyState(static_cast<int>(wParam_), Input::InputState::Released);
		return 0;
	}
	case WM_KEYDOWN:
	{
		Input::SetKeyState(static_cast<int>(wParam_), Input::InputState::Pressed);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		Input::SetMouseButtonState(0, Input::InputState::Released);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		Input::SetMouseButtonState(0, Input::InputState::Pressed);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		Input::SetMouseButtonState(1, Input::InputState::Released);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		Input::SetMouseButtonState(1, Input::InputState::Pressed);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		Input::SetMouseButtonState(2, Input::InputState::Released);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		Input::SetMouseButtonState(2, Input::InputState::Pressed);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		POINTS mousePos = MAKEPOINTS(lParam_);
		Input::SetMousePosition(Vector2D((float)mousePos.x, (float)mousePos.y));
		return 0;
	}
	case WM_SIZE:
	{
		const int width = LOWORD(lParam_);
		const int height = HIWORD(lParam_);

		if (width > 0 && height > 0)
		{
			Renderer::Resize(width, height);
			return 0;
		}

		Logger::Warning("윈도우 크기가 유효하지 않습니다! width: %d, height: %d", width, height);
		return 0;
	}
	case WM_ERASEBKGND:
	{
		return 1;
	}
	case WM_CLOSE:
	{
		DestroyWindow(hWnd_);
		return 0;
	}
	case WM_DESTROY:
	{
		SetShouldClose(true);
		return 0;
	}
	default:;
	}
	return DefWindowProcW(hWnd_, uMsg_, wParam_, lParam_);
}

Window::Options Window::options{};
HWND Window::hWnd{ nullptr };
bool Window::shouldClose{ false };
