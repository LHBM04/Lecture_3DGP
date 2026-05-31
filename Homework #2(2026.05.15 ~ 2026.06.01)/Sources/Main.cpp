#include "Precompiled.h"

#include "InputSystem.h"
#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "SceneSystem.h"
#include "Scene_Stage1.h"
#include "Scene_Title.h"

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
		0, WindowClassName, WindowTitle, WindowStyle,
		windowPosX, windowPosY, WindowWidth, WindowHeight,
		nullptr, nullptr, hInstance, nullptr) };

	if (mainWindow == nullptr) return -1;

	InputSystem::GetInstance().Reset();
	ResourceSystem::GetInstance().Initialize();

	if (RenderSystem::GetInstance().Initialize(mainWindow).has_value() == false) return -1;

	if (IsWindowVisible(mainWindow) == false)
	{
		ShowWindow(mainWindow, nCmdShow);
		UpdateWindow(mainWindow);
	}

	SceneSystem::GetInstance().AddScene(L"Title Scene", std::make_unique<Scene_Title>());
	SceneSystem::GetInstance().AddScene(L"Stage 1", std::make_unique<Scene_Stage1>());
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

		{
			LARGE_INTEGER nowTime{};
			QueryPerformanceCounter(&nowTime);
			float deltaTime{ static_cast<float>(nowTime.QuadPart - lastTime.QuadPart) / static_cast<float>(frequency.QuadPart) };

			if ((fixedTime += deltaTime) >= 2.0f) fixedTime = 2.0f;

			while (fixedTime >= fixedStep)
			{
				fixedTime -= fixedStep;
				SceneSystem::GetInstance().FixedUpdate(fixedStep);
			}

			SceneSystem::GetInstance().Update(deltaTime);
			SceneSystem::GetInstance().LateUpdate(deltaTime);
			lastTime = nowTime;
		}

		if (RenderSystem::GetInstance().BeginFrame())
		{
			SceneSystem::GetInstance().Render();
			RenderSystem::GetInstance().EndFrame();
			RenderSystem::GetInstance().Present();
		}
	}

	RenderSystem::GetInstance().Release();
	ResourceSystem::GetInstance().Release();

	if (mainWindow != nullptr)
	{
		DestroyWindow(mainWindow);
	}
	UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

	return static_cast<INT>(msg.wParam);
}
