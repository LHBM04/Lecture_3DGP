#include "Precompiled.h"
#include "Application.h"

#include "InputManager.h"
#include "Scene_Test.h"
#include "Scene_Title.h"
#include "SceneManager.h"
#include "SceneSystem.h"
#include "Timer.h"

#define SCENE_ENTRY(SceneType, SceneName) \
    SceneBuildEntry{ SceneName, []() -> std::unique_ptr<Scene> { return std::make_unique<SceneType>(); } }

namespace
{
	bool isRunning;

	Window window;
	Renderer renderer;
	SceneSystem sceneSystem;
}

bool Application::Initialize(const ApplicationOptions& options_)
{
	// 창 생성 및 초기화.
	{
		DWORD style{ WS_OVERLAPPEDWINDOW };
		if (options_.borderless)
		{
			style = WS_POPUP;
		}
		else if (!options_.resizable)
		{
			style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		}

		int windowX{ 0 };
		int windowY{ 0 };
		int windowWidth{ options_.width };
		int windowHeight{ options_.height };

		WindowOptions options{};
		options.title = options_.title;
		options.x = windowX;
		options.y = windowY;
		options.width = windowWidth;
		options.height = windowHeight;
		options.style = style;
		options.styleEx = WS_EX_APPWINDOW;

		if (!window.Initialize(options))
		{
			return false;
		}

		window.Show();
	}

	// 렌더러 생성 및 초기화.
	{
		RendererOptions options{};
		options.window = window.GetHandle();
		options.width = window.GetWidth();
		options.height = window.GetHeight();
		options.msaa4xEnable = false;
		options.enableTripleBuffering = false;
		options.vSync = true;
		options.fullscreen = options_.fullscreen;

		if (!renderer.Initialize(options))
		{
			return false;
		}
	}

	InputManager::Reset();
	InputManager::SetScreenSize(window.GetWidth(), window.GetHeight());
	Timer::Reset();

	SceneOptions options{
		.scenes =
		{
			SCENE_ENTRY(Scene_Test,  L"Title"),
			SCENE_ENTRY(Scene_Title, L"Game")
		},
		.startIndex = 0
	};

	sceneSystem.Initialize(options);

	isRunning = false;
	
	return true;
}

int Application::Run()
{
	isRunning = true;

	Event event;
	while (isRunning)
	{
		Timer::Tick();
		InputManager::Update();

		while (window.PollEvent(event))
		{
			switch (event.type)
			{
			case Event::Type::WindowResize:
				InputManager::SetScreenSize(event.resize.width, event.resize.height);
				break;
			case Event::Type::WindowFullscreenToggle:
				renderer.ToggleFullscreen();
				InputManager::SetScreenSize(renderer.GetWidth(), renderer.GetHeight());
				break;
			case Event::Type::WindowClose:
				isRunning = false;
				break;
			default:
				break;
			}

			InputManager::ProcessEvent(event);
		}

		sceneSystem.Update();

		renderer.BeginRender();
		sceneSystem.Render();
		renderer.EndRender();
	}

	sceneSystem.Release();
	renderer.Release();
	window.Release();

	return 0;
	// return event.quit.quitCode;
}

void Application::Quit()
{
	isRunning = false;
}

Window& Application::GetWindow()
{
	return window;
}

Renderer& Application::GetRenderer()
{
	return renderer;
}

SceneSystem& Application::GetSceneSystem()
{
	return sceneSystem;
}
