#include "Precompiled.h"
#include "Application.h"

#undef CreateWindow

LRESULT CALLBACK Engine::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	Engine* app{ nullptr };

	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCTW* createStruct{ reinterpret_cast<const CREATESTRUCTW*>(lParam) };
		app = reinterpret_cast<Engine*>(createStruct->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<Engine*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	if (app != nullptr)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
			app->SetShouldClose(true);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			app->Resize(LOWORD(lParam), HIWORD(lParam));
			return 0;
		default:
			break;
		}
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

DWORD Engine::ResolveWindowStyle() const noexcept
{
	if (options_.fullscreen || options_.borderless)
	{
		return WS_POPUP | WS_VISIBLE;
	}

	if (options_.resizable)
	{
		return WS_OVERLAPPEDWINDOW;
	}

	return WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
}

bool Engine::Initialize(HINSTANCE instance__) noexcept
{
	instance_ = instance__;

	WNDCLASSEXW wc
	{
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WindowProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = instance_,
		.hIcon = LoadIconW(nullptr, IDI_APPLICATION),
		.hCursor = LoadCursorW(nullptr, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = nullptr,
		.lpszClassName = WINDOW_CLASS_NAME,
		.hIconSm = LoadIconW(nullptr, IDI_APPLICATION)
	};

	return RegisterClassExW(&wc) != 0;
}

int Engine::Run(const Options& options)
{
	options_ = options;
	
	MSG message{};
	while (!ShouldClose())
	{
		if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				SetShouldClose(true);
				break;
			}

			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}

	return static_cast<int>(message.wParam);
}

bool Engine::ShouldClose() const noexcept
{
	return shouldClose_;
}

void Engine::SetShouldClose(bool shouldClose__) noexcept
{
	shouldClose_ = shouldClose__;
}

LPCWSTR Engine::GetTitle() const noexcept
{
	return options_.title;
}

void Engine::SetTitle(LPCWSTR title_) noexcept
{
	options_.title = title_;
	if (window_ != nullptr)
	{
		SetWindowTextW(window_, title_);
	}
}

int Engine::GetWidth() const noexcept
{
	return options_.width;
}

void Engine::SetWidth(int width_) noexcept
{
	options_.width = width_;
	ApplyWindowMode();
}

int Engine::GetHeight() const noexcept
{
	return options_.height;
}

void Engine::SetHeight(int height_) noexcept
{
	options_.height = height_;
	ApplyWindowMode();
}

bool Engine::IsResizable() const noexcept
{
	return options_.resizable;
}

void Engine::SetResizable(bool resizable_) noexcept
{
	options_.resizable = resizable_;
	ApplyWindowMode();
}

bool Engine::IsFullscreen() const noexcept
{
	return options_.fullscreen;
}

void Engine::SetFullscreen(bool fullscreen_) noexcept
{
	options_.fullscreen = fullscreen_;
	if (fullscreen_)
	{
		options_.borderless = false;
	}
	ApplyWindowMode();
}

bool Engine::IsBorderless() const noexcept
{
	return options_.borderless;
}

void Engine::SetBorderless(bool borderless_) noexcept
{
	options_.borderless = borderless_;
	if (borderless_)
	{
		options_.fullscreen = false;
	}
	ApplyWindowMode();
}

void Engine::Resize(int width_, int height_) noexcept
{
	options_.width = std::max(1, width_);
	options_.height = std::max(1, height_);
}

void Engine::ApplyWindowMode() noexcept
{
	if (window_ == nullptr)
	{
		return;
	}

	const DWORD style{ ResolveWindowStyle() };
	SetWindowLongPtrW(window_, GWL_STYLE, static_cast<LONG_PTR>(style));

	if (options_.fullscreen)
	{
		SetWindowPos(
			window_,
			HWND_TOP,
			0,
			0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE
		);
		return;
	}

	RECT rect
	{
		.left = 0,
		.top = 0,
		.right = static_cast<LONG>(options_.width),
		.bottom = static_cast<LONG>(options_.height)
	};
	AdjustWindowRectEx(&rect, style, FALSE, 0);
	SetWindowPos(
		window_,
		nullptr,
		0,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE
	);
}
