#include "Precompiled.h"
#include "Engine.h"

#include "RenderService.h"
#include "ResourceService.h"
#include "SceneService.h"
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

	const int result{ engine.Run() };
	engine.Shutdown();
	return result;
}

bool Engine::Initialize()
{
	// 윈도우 시스템 추가
	{
		WindowService& windowService{ AddService<WindowService>() };

		WindowOptions mainOptions{};
		mainOptions.width = GetOption(L"Window.Width", 800);
		mainOptions.height = GetOption(L"Window.Height", 600);
		mainOptions.title = GetOption(L"Window.Title", L"New Window");
		mainOptions.x = 0;
		mainOptions.y = 0;

		HWND mainWindow{ windowService.AddWindow(mainOptions) };
		if (mainWindow == nullptr)
		{
			return false;
		}

		windowService.SetMainWindow(mainWindow);
	}
	// 렌더 시스템 추가
	{
		RenderService& renderService{ AddService<RenderService>() };

		renderService.AddTarget(GetService<WindowService>().GetMainWindow());
	}
	// 리소스/씬 서비스는 구체 리소스 로더나 씬 구현을 몰라야 합니다.
	{
		AddService<ResourceService>();
		AddService<SceneService>();
	}

	return true;
}

void Engine::Shutdown()
{
	for (auto it{ services.rbegin() }; it != services.rend(); ++it)
	{
		(*it)->NotifyRemove();
	}
	services.clear();
}

int Engine::Run()
{
	WindowService& windowService{ GetService<WindowService>() };
	RenderService& renderService{ GetService<RenderService>() };
	SceneService& sceneService{ GetService<SceneService>() };

	ApplicationRequest applicationRequest;
	SceneRequest sceneRequest;
	RenderContext renderContext;
	TimeContext timeContext;

	auto previousTime{ std::chrono::steady_clock::now() };
	float totalTime{ 0.0f };

	bool shouldClose{ false };
	while (!shouldClose)
	{
		applicationRequest.Clear();
		sceneRequest.Clear();

		const auto currentTime{ std::chrono::steady_clock::now() };
		const std::chrono::duration<float> deltaTime{ currentTime - previousTime };
		previousTime = currentTime;
		totalTime += deltaTime.count();

		timeContext.deltaTime = deltaTime.count();
		timeContext.unscaledDeltaTime = deltaTime.count();
		timeContext.totalTime = totalTime;

		sceneService.SetRequests(applicationRequest, sceneRequest);

		if (!windowService.PollEvents())
		{
			shouldClose = true;
			break;
		}

		sceneService.Update(timeContext);

		// 지연 실행은 Update 이후에 처리합니다.
		// 컴포넌트 순회 중 씬 상태를 바꾸면 반복자가 깨질 수 있기 때문입니다.
		if (applicationRequest.IsQuitRequested())
		{
			break;
		}
		if (sceneRequest.HasSceneChangeRequest())
		{
			if (sceneRequest.IsUnloadRequested())
			{
				sceneService.UnloadScene();
			}
			else
			{
				sceneService.LoadScene(sceneRequest.GetPendingSceneLoad());
			}
		}

		renderService.BeginFrame();

		for (HWND window : windowService.GetWindows())
		{
			renderContext.Clear();
			sceneService.Render(renderContext);
			renderService.Render(window, renderContext);
		}

		renderService.EndFrame();
	}

	return true;
}
