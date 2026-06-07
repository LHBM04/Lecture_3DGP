#include "Precompiled.h"

#include <fcntl.h>
#include <io.h>

#include "InputSystem.h"
#include "Logger.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "Scene_Level1.h"
#include "Scene_Level2.h"
#include "Scene_Level3.h"
#include "Scene_Title.h"
#include "SceneSystem.h"
#include "TimeSystem.h"

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HWND mainWindow{ nullptr };
constexpr LPCWSTR WindowClassName{ L"Homework #3 Class" };
constexpr LPCWSTR WindowTitle{ L"Homework #3(2026.05.18 ~ 2026.06.08)" };
constexpr int WindowWidth{ 800 };
constexpr int WindowHeight{ 600 };
constexpr DWORD WindowStyle{ WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX };

bool isRunning{ false };

INT APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ INT nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);

#if defined(_DEBUG)
	assert(::AllocConsole());

	FILE* consoleStream{ nullptr };
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);

	Logger::Info(L"콘솔 출력 초기화가 완료되었습니다.");
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
	::RegisterClassExW(&wndClass);

	const int screenWidth{ ::GetSystemMetrics(SM_CXSCREEN) };
	const int screenHeight{ ::GetSystemMetrics(SM_CYSCREEN) };
	const int windowPosX{ (screenWidth - WindowWidth) / 2 };
	const int windowPosY{ (screenHeight - WindowHeight) / 2 };

	mainWindow = ::CreateWindowExW(
		0, 
		WindowClassName, 
		WindowTitle, 
		WindowStyle,
		windowPosX, windowPosY, 
		WindowWidth, WindowHeight,
		nullptr, 
		nullptr, 
		hInstance, 
		nullptr);

	if (mainWindow == nullptr)
	{
		Logger::Critical(L"메인 윈도우를 생성하지 못했습니다.");
		return -1;
	}

	if (!::IsWindowVisible(mainWindow))
	{
		::ShowWindow(mainWindow, nCmdShow);
		::UpdateWindow(mainWindow);
	}

	Logger::Info(L"RenderSystem 초기화를 시작합니다.");
	if (!RenderSystem::GetInstance().Initialize(mainWindow))
	{
		Logger::Critical(L"RenderSystem 초기화에 실패했습니다. 프로그램을 종료합니다.");
		return -1;
	}
	Logger::Info(L"RenderSystem 초기화가 완료되었습니다.");

	Logger::Info(L"ResourceSystem 초기화를 시작합니다.");
	ResourceSystem::GetInstance().Initialize();
	Logger::Info(L"ResourceSystem 초기화가 완료되었습니다.");

	SceneSystem::GetInstance().AddScene(L"Title", std::make_unique<Scene_Title>());
	SceneSystem::GetInstance().AddScene(L"Level1", std::make_unique<Scene_Level1>());
	SceneSystem::GetInstance().AddScene(L"Level2", std::make_unique<Scene_Level2>());
	SceneSystem::GetInstance().AddScene(L"Level3", std::make_unique<Scene_Level3>());
	SceneSystem::GetInstance().LoadScene(L"Title");

	Logger::Info(L"InputSystem 초기화를 시작합니다.");
	InputSystem::GetInstance().Reset();
	Logger::Info(L"InputSystem 초기화가 완료되었습니다.");

	Logger::Info(L"PhysicsSystem 초기화를 시작합니다.");
	PhysicsSystem::GetInstance().Reset();
	Logger::Info(L"PhysicsSystem 초기화가 완료되었습니다.");

	Logger::Info(L"TimeSystem 초기화를 시작합니다.");
	TimeSystem::GetInstance().Reset();
	Logger::Info(L"TimeSystem 초기화가 완료되었습니다.");

	isRunning = true;

	MSG msg;
	while (isRunning)
	{
		while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		if (!isRunning)
		{
			break;
		}

		TimeSystem& timeSystem{ TimeSystem::GetInstance() };
		timeSystem.Tick();

		while (timeSystem.GetFixedTime() >= timeSystem.GetFixedDeltaTime())
		{
			PhysicsSystem::GetInstance().Update();
			SceneSystem::GetInstance().FixedUpdate();

			timeSystem.GetFixedTime() -= timeSystem.GetFixedDeltaTime();
		}

		InputSystem::GetInstance().Update();
		SceneSystem::GetInstance().Update();
		SceneSystem::GetInstance().Render();
	}

	SceneSystem::GetInstance().Release();
	ResourceSystem::GetInstance().Release();
	RenderSystem::GetInstance().Release();

	if (mainWindow != nullptr && ::IsWindow(mainWindow))
	{
		::DestroyWindow(mainWindow);
		mainWindow = nullptr;
	}

	::UnregisterClassW(wndClass.lpszClassName, wndClass.hInstance);

	return 0;
}
