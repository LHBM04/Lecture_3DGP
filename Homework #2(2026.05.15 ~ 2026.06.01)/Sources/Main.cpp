#include "Precompiled.h"

#include "RenderSystem.h"
#include "InputSystem.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	std::unreachable(); // 도달 불가.
}

constexpr LPCWSTR WindowClassName{ L"Homework #2 Class" };

constexpr LPCWSTR WindowTitle{ L"Homework #2(2026.05.15 ~ 2026.06.01)" };
constexpr int WindowWidth{ 800 };
constexpr int WindowHeight{ 600 };

bool isRunning{ false };

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASSEXW wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = WindowClassName;
	wndClass.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
	RegisterClassExW(&wndClass);

	HWND mainWindow = CreateWindowExW(
		0,
		wndClass.lpszClassName,
		WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight,
		nullptr, nullptr, hInstance, nullptr);

	if (mainWindow == nullptr)
	{
		return -1;
	}

	ShowWindow(mainWindow, nCmdShow);
	UpdateWindow(mainWindow);

	if (!RenderSystem::GetInstance().Initialize(mainWindow))
	{
		return -1;
	}

	InputSystem::GetInstance().Reset();

	isRunning = true;

	MSG msg;
	while (isRunning)
	{
		InputSystem::GetInstance().Update();

		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		RenderSystem::GetInstance().BeginFrame();
		RenderSystem::GetInstance().Clear();
		RenderSystem::GetInstance().EndFrame();
		RenderSystem::GetInstance().Present();
	}

	RenderSystem::GetInstance().Release();

	if (mainWindow != nullptr)
	{
		DestroyWindow(mainWindow);
		mainWindow = nullptr;
	}

	UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

	return (INT)msg.wParam;
}