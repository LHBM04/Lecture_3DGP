#include "Precompiled.h"

#include "Logger.h"
#include "RenderSystem.h"

LRESULT CALLBACK WindowProc(
	_In_ HWND hwnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	default:
		return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	std::unreachable();
}

constexpr LPCWSTR WindowClassName{ L"Homework #3 Class" };

constexpr LPCWSTR WindowTitle{ L"Homework #3(2026.05.18 ~ 2026.06.08)" };
constexpr int WindowWidth{ 800 };
constexpr int WindowHeight{ 600 };
constexpr DWORD WindowStyle{ WS_OVERLAPPEDWINDOW };
HWND mainWindow{ nullptr };

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ INT nCmdShow)
{
#if defined(_DEBUG)
	if (!::AllocConsole())
	{
		::MessageBoxW(nullptr, L"Cannot open the console stream!", L"Oops!", NULL);
		return -1;
	}

	FILE* consoleStream{ nullptr };
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);
#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	WNDCLASSEXW wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
	wndClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wndClass.lpszClassName = TEXT("Homework #3 Class");
	::RegisterClassExW(&wndClass);

	const int screenWidth{ ::GetSystemMetrics(SM_CXSCREEN) };
	const int screenHeight{ ::GetSystemMetrics(SM_CYSCREEN) };
	const int windowPosX{ (screenWidth - WindowWidth) / 2 };
	const int windowPosY{ (screenHeight - WindowHeight) / 2 };

	mainWindow = ::CreateWindowExW(
		0,
		L"Homework #3 Class",
		L"Homework #3(2026.05.18 ~ 2026.06.08)",
		WindowStyle,
		windowPosX, windowPosY,
		WindowWidth, WindowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (mainWindow == nullptr)
	{
		Logger::Critical(L"[Init] CreateWindowExW failed");
		return -1;
	}

	::ShowWindow(mainWindow, nCmdShow);
	::UpdateWindow(mainWindow);

	if (!RenderSystem::GetInstance().Initialize(mainWindow))
	{
		::MessageBoxW(mainWindow, L"Failed to initialize Render System.", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	MSG msg;
	while (true)
	{
		if (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
		else
		{
			// Game loop goes here.
		}
	}

	RenderSystem::GetInstance().Release();

	if (!::DestroyWindow(mainWindow))
	{
		Logger::Critical(L"[Release] DestroyWindow failed");
		return -1;
	}

	::UnregisterClassW(WindowClassName, hInstance);

	return static_cast<int>(msg.wParam);
}
