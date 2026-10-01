#include "Precompiled.h"
#include "Application.h"

#ifdef RegisterClass
#undef RegisterClass
#endif

#ifdef CreateWindow
#undef CreateWindow
#endif

#ifdef UnregisterClass
#undef UnregisterClass
#endif

namespace
{
	/** HINSTANCE 캐시 */
	HINSTANCE hInstance = nullptr;

	/** WNDCLASSEX 등록 이름 */
	LPCTSTR ClassName = TEXT("Homework #4(2026.05.18 ~ 2026.06.17) Class");

	/** 애플리케이션 가동 여부 */
	bool isRunning = false;

	/** HWND 캐시 */
	HWND hWnd = nullptr;

	LRESULT CALLBACK WndProc(
		_In_ HWND hWnd,
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam)
	{
		if (uMsg == WM_DESTROY)
		{
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	std::expected<void, std::string> RegisterClass()
	{
		WNDCLASSEX wndClass{};
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wndClass.lpszClassName = ClassName;

		if (RegisterClassEx(&wndClass) == 0)
		{
			const DWORD errorCode = GetLastError();
			return std::unexpected(std::format("Failed to register window class. Error code: {}", errorCode));
		}
	}

	std::expected<void, std::string> CreateWindow(const Application::Options& options)
	{
		hWnd = CreateWindowEx(
			0,
			ClassName,
			options.title,
			options.style,
			options.x,
			options.y,
			options.width,
			options.height,
			nullptr,
			nullptr,
			hInstance,
			nullptr);

		if (!hWnd)
		{
			const DWORD errorCode = GetLastError();
			return std::unexpected(std::format("Failed to create window. Error code: {}", errorCode));
		}

		return {};
	}

	void UnregisterClass()
	{
		if (hInstance)
		{
			UnregisterClassW(ClassName, hInstance);
			hInstance = nullptr;
		}
	}
}

int Application::Run(const Options& options)
{
	hInstance = GetModuleHandle(nullptr);

	// WNDCLASSEX 등록
	if (auto result = RegisterClass(); !result)
	{
		MessageBoxExA(nullptr, result.error().c_str(), "Oops!", MB_ICONERROR, 0);
		PostQuitMessage(EXIT_FAILURE);
	}

	// HWND 생성
	if (auto result = CreateWindow(options); !result)
	{
		MessageBoxExA(nullptr, result.error().c_str(), "Oops!", MB_ICONERROR, 0);
		PostQuitMessage(EXIT_FAILURE);
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	isRunning = true;

	MSG msg{};
	while (true)
	{
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

			if (msg.message == WM_QUIT)
			{
				isRunning = false;
			}
		}

		if (!isRunning)
		{
			break;
		}
	}

	if (hWnd)
	{
		DestroyWindow(hWnd);
		hWnd = nullptr;
	}

	UnregisterClass();

	return static_cast<int>(msg.wParam);
}

HWND Application::GetHWND() noexcept
{
	return hWnd;
}

bool Application::IsRunning()
{
	return isRunning;
}

void Application::SetRunning(bool running)
{
	isRunning = running;
}
