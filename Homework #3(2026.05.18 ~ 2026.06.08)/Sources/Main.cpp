#include "Precompiled.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

constexpr LPCWSTR WindowClassName{ L"Homework #3 Class" };
constexpr LPCWSTR WindowTitle{ L"Homework #3(2026.05.18 ~ 2026.06.08" };
constexpr INT WindowWidth{ 800 };
constexpr INT WindowHeight{ 600 };
constexpr DWORD WindowStyle{ WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX };

LARGE_INTEGER frequency;
LARGE_INTEGER lastTime;

constexpr float FixedStep{ 1.0f / 60.0f };
float fixedTime{ 0.0f };

bool isRunning{ false };

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if defined(_DEBUG)
	if (!::AllocConsole())
	{
		::MessageBoxW(nullptr, L"Cannot open console stream!", L"Oops!", NULL);
		return -1;
	}

	FILE* consoleStream{ nullptr };
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);
#endif

	WNDCLASSEXW wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wndClass.lpszClassName = WindowClassName;
	RegisterClassExW(&wndClass);

	const int screenWidth{ GetSystemMetrics(SM_CXSCREEN) };
	const int screenHeight{ GetSystemMetrics(SM_CYSCREEN) };
	const int windowPosX{ (screenWidth - WindowWidth) / 2 };
	const int windowPosY{ (screenHeight - WindowHeight) / 2 };

	HWND mainWindow{ CreateWindowExW(
		0, 
		WindowClassName, 
		WindowTitle, 
		WindowStyle,
		windowPosX, windowPosY, 
		WindowWidth, WindowHeight,
		nullptr, 
		nullptr, 
		hInstance, 
		nullptr) 
	};

	if (mainWindow == nullptr)
	{
		::MessageBoxW(nullptr, L"Cannot intialize main window!", L"Oops!", NULL);
		return -1;
	}

	isRunning = true;

	if (!::IsWindowVisible(mainWindow))
	{
		::ShowWindow(mainWindow, nCmdShow);
		::UpdateWindow(mainWindow);
	}

	MSG msg{};
	while (isRunning)
	{
		while (::PeekMessageW(&msg, mainWindow, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		// Update
		{
			LARGE_INTEGER nowTime{};
			::QueryPerformanceCounter(&nowTime);

			const float deltaTime{ static_cast<float>(nowTime.QuadPart - lastTime.QuadPart) / static_cast<float>(frequency.QuadPart) };

			if ((fixedTime += deltaTime) >= 2.0f)
			{
				fixedTime = 2.0f;
			}

			while (fixedTime >= FixedStep)
			{
				fixedTime -= FixedStep;
			}

			lastTime = nowTime;
		}

		// Render
		{

		}
	}

	if (mainWindow != nullptr)
	{
		::DestroyWindow(mainWindow);
		mainWindow = nullptr;
	}

	UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

	return static_cast<INT>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}

	std::unreachable();
}
