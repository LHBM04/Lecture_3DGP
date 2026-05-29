#include "Precompiled.h"

#include "RenderSystem.h"

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
}

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
	wndClass.hInstance = hInstance;
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszClassName = L"Homework #2 Class";
	RegisterClassExW(&wndClass);

	HWND mainWindow = CreateWindowExW(
		0,
		wndClass.lpszClassName,
		L"Homework #2(2026.05.15 ~ 2026.06.01)",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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

	isRunning = true;

	MSG msg;
	while (isRunning)
	{
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

		RenderSystem::GetInstance().Clear();
		RenderSystem::GetInstance().Present();
	}

	return (INT)msg.wParam;
}