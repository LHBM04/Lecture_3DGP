#include "Precompiled.h"

#include "InputSystem.h"
#include "Logger.h"
#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "Scene_Menu.h"
#include "SceneSystem.h"
#include "Scene_Stage1.h"
#include "Scene_Stage2.h"
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

	if (mainWindow == nullptr)
	{
		Logger::Critical(L"[Init] CreateWindowExW failed");
		return -1;
	}

	Logger::Trace(L"[Init] InputSystem.Reset begin");
	InputSystem::GetInstance().Reset();
	Logger::Info(L"[Init] InputSystem.Reset success");

	Logger::Trace(L"[Init] ResourceSystem.Initialize begin");
	ResourceSystem::GetInstance().Initialize();
	Logger::Info(L"[Init] ResourceSystem.Initialize success");

	Logger::Trace(L"[Init] RenderSystem.Initialize begin");
	if (std::expected<void, std::wstring> renderInitResult{ RenderSystem::GetInstance().Initialize(mainWindow) }; !renderInitResult.has_value())
	{
		Logger::Critical(L"[Init] RenderSystem.Initialize failed: {}", renderInitResult.error());
		return -1;
	}
	Logger::Info(L"[Init] RenderSystem.Initialize success");

	if (IsWindowVisible(mainWindow) == false)
	{
		ShowWindow(mainWindow, nCmdShow);
		UpdateWindow(mainWindow);
	}

	Logger::Trace(L"[Init] SceneSystem.AddScene begin");
	SceneSystem::GetInstance().AddScene(L"Title Scene", std::make_unique<Scene_Title>());
	SceneSystem::GetInstance().AddScene(L"Menu Scene", std::make_unique<Scene_Menu>());
	SceneSystem::GetInstance().AddScene(L"Stage 1", std::make_unique<Scene_Stage1>());
	SceneSystem::GetInstance().AddScene(L"Stage 2", std::make_unique<Scene_Stage2>());
	SceneSystem::GetInstance().LoadScene(L"Title Scene");
	Logger::Info(L"[Init] SceneSystem.AddScene/LoadScene success");
	
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);

	Logger::Info(L"[Init] Main loop start");
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
			lastTime = nowTime;
		}

		if (RenderSystem::GetInstance().BeginFrame())
		{
			SceneSystem::GetInstance().Render();
			RenderSystem::GetInstance().EndFrame();
			RenderSystem::GetInstance().Present();
		}
	}

	Logger::Trace(L"[Shutdown] RenderSystem.Release begin");
	RenderSystem::GetInstance().Release();
	Logger::Info(L"[Shutdown] RenderSystem.Release success");

	Logger::Trace(L"[Shutdown] ResourceSystem.Release begin");
	ResourceSystem::GetInstance().Release();
	Logger::Info(L"[Shutdown] ResourceSystem.Release success");

	if (mainWindow != nullptr)
	{
		DestroyWindow(mainWindow);
	}
	UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

	return static_cast<INT>(msg.wParam);
}
