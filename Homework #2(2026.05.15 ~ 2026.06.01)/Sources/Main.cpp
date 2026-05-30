#include "Precompiled.h"

#include "InputSystem.h"
#include "RenderSystem.h"
#include "Scene_Title.h"
#include "SceneSystem.h"

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
constexpr DWORD WindowStyle{ WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX };

LARGE_INTEGER frequency;
LARGE_INTEGER lastTime;

float fixedStep{ 1.0f / 60.0f };
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
	assert(AllocConsole());
	
	FILE* consoleStream{ nullptr };
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);

	// Logger::Info("Console allocated for debugging.");
#endif

	WNDCLASSEXW wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEXW);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = WindowClassName;
	wndClass.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
	RegisterClassExW(&wndClass);

	HWND mainWindow = CreateWindowExW(
		0,
		WindowClassName,
		WindowTitle,
		WindowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight,
		nullptr, nullptr, hInstance, nullptr);

	if (mainWindow == nullptr)
	{
		// Logger::Critical("Failed to create main window.");
		return -1;
	}
	
	if (auto result = RenderSystem::GetInstance().Initialize(mainWindow); !result)
	{
		// Logger::Critical(result.error());
		return -1;
	}

	if (!IsWindowVisible(mainWindow))
	{
		ShowWindow(mainWindow, nCmdShow);
		UpdateWindow(mainWindow);
	}

	InputSystem::GetInstance().Reset();

	SceneSystem::GetInstance().AddScene(L"Title Scene", std::make_unique<Scene_Title>());
	SceneSystem::GetInstance().LoadScene(L"Title Scene");
	
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);

	isRunning = true;

	MSG msg{};
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

		// 게임 업데이트.
		{
			LARGE_INTEGER nowTime{};
			QueryPerformanceCounter(&nowTime);

			LARGE_INTEGER elapsedTicks{};
			elapsedTicks.QuadPart = nowTime.QuadPart - lastTime.QuadPart;
			float deltaTime{ static_cast<float>(elapsedTicks.QuadPart) / static_cast<float>(frequency.QuadPart) };

			if ((fixedTime += deltaTime) >= 2.0f)
			{
				fixedTime = 2.0f;
			}

			while (fixedTime >= fixedStep)
			{
				fixedTime -= fixedStep;
				SceneSystem::GetInstance().FixedUpdate(fixedStep);
			}

			SceneSystem::GetInstance().Update(deltaTime);
			lastTime = nowTime;
		}
		// 게임 렌더.
		{
			RenderSystem::GetInstance().BeginFrame();
			SceneSystem::GetInstance().Render();
			RenderSystem::GetInstance().EndFrame();
			RenderSystem::GetInstance().Present();
		}
	}

	RenderSystem::GetInstance().Release();

	if (mainWindow != nullptr)
	{
		DestroyWindow(mainWindow);
		mainWindow = nullptr;
	}

	UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

	return static_cast<INT>(msg.wParam);
}

