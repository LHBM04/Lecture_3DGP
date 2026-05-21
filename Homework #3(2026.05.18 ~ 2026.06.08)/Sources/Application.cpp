#include "Precompiled.h"
#include "Application.h"

namespace
{
	Application application;
}

bool Application::Initialize(HINSTANCE instance, const Options& options)
{
	Options resolvedOptions{ options };
	resolvedOptions.instance = instance;
	return application.InitializeInstance(resolvedOptions);
}

int Application::Run()
{
	return application.RunInstance();
}

void Application::Shutdown() noexcept
{
	application.ShutdownInstance();
}

LRESULT CALLBACK Application::WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		::DestroyWindow(windowHandle);
		return 0;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	default:
		return ::DefWindowProcW(windowHandle, message, wParam, lParam);
	}
}

bool Application::InitializeInstance(const Options& options)
{
	instance = options.instance;
	width = options.width;
	height = options.height;

	if (!CreateMainWindow(options))
	{
		return false;
	}

	if (!renderer.Initialize(windowHandle, width, height))
	{
		ShutdownInstance();
		return false;
	}

	::ShowWindow(windowHandle, options.showCommand);
	::UpdateWindow(windowHandle);

	running = true;
	return true;
}

int Application::RunInstance()
{
	MSG message{};

	while (running)
	{
		while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				running = false;
				break;
			}

			::TranslateMessage(&message);
			::DispatchMessageW(&message);
		}

		if (running)
		{
			renderer.Render();
		}
	}

	ShutdownInstance();
	return static_cast<int>(message.wParam);
}

void Application::ShutdownInstance() noexcept
{
	renderer.Shutdown();

	if (windowHandle != nullptr)
	{
		::DestroyWindow(windowHandle);
		windowHandle = nullptr;
	}

	instance = nullptr;
	width = 0;
	height = 0;
	running = false;
}

bool Application::CreateMainWindow(const Options& options)
{
	constexpr LPCWSTR className{ L"Homework3DX12Window" };

	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &Application::WindowProcedure;
	windowClass.hInstance = instance;
	windowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	windowClass.lpszClassName = className;

	if (::RegisterClassExW(&windowClass) == 0)
	{
		return false;
	}

	RECT windowRect{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	if (!::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE))
	{
		return false;
	}

	windowHandle = ::CreateWindowExW(
		0,
		className,
		options.title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		instance,
		nullptr);

	return windowHandle != nullptr;
}
