#include "Precompiled.h"
#include "Engine.h"

#include "RenderService.h"
#include "WindowService.h"

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

	Engine engine;

	// 애플리케이션 설정
	{
		engine.SetOption(L"App.Instance", hInstance);
	}
	// 창 설정
	{
		engine.SetOption(L"Window.Width", 800);
		engine.SetOption(L"Window.Height", 600);
		engine.SetOption(L"Window.Title", L"Homework #3(2026.05.18 ~ 2026.06.08)");
	}
	// 렌더러 설정
	{
		engine.SetOption(L"Render.VSync", true);
	}

	if (!engine.Initialize())
	{
		::MessageBoxW(nullptr, L"Cannot initialize the engine!", L"Oops!", NULL);
		return -1;
	}

	return engine.Run();
}

bool Engine::Initialize()
{
	// 윈도우 시스템 추가
	{
		windowService = &AddService<WindowService>();

		WindowOptions mainOptions{};
		mainOptions.width = GetOption(L"Window.Width", 800);
		mainOptions.height = GetOption(L"Window.Height", 600);
		mainOptions.title = GetOption(L"Window.Title", L"New Window");
		mainOptions.x = 0;
		mainOptions.y = 0;

		HWND mainWindow{ windowService->AddWindow(mainOptions) };
		if (mainWindow == nullptr)
		{
			return false;
		}
		windowService->SetMainWindow(mainWindow);
	}
	// 렌더 시스템 추가
	{
		renderService = &AddService<RenderService>();

		renderService->AddTarget(windowService->GetMainWindow());
	}

	return true;
}

void Engine::Shutdown()
{

}

int Engine::Run()
{
	WindowService& windowService{ GetService<WindowService>() };
	RenderService& renderService{ GetService<RenderService>() };

	bool shouldClose{ false };
	while (!shouldClose)
	{
		if (!windowService.PollEvents())
		{
			shouldClose = true;
			break;
		}

		renderService.Render();
	}

	return true;
}
