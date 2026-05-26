#include "Precompiled.h"
#include "Application.h"

#include "InputManager.h"
#include "PlayerInputMapper.h"
#include "RenderContext.h"
#include "Scene_Test.h"
#include "Scene_Title.h"
#include "SceneManager.h"
#include "SceneSystem.h"
#include "Timer.h"

#define SCENE_ENTRY(SceneType, SceneName) \
    SceneBuildEntry{ SceneName, []() -> std::unique_ptr<Scene> { return std::make_unique<SceneType>(); } }

namespace
{
	constexpr int MaxFixedUpdatesPerFrame{ 5 };

	bool isRunning;
	bool isInitialized;

	WindowSystem windowSystem;
	RenderSystem renderSystem;
	SceneSystem sceneSystem;
	RenderTargetHandle mainRenderTarget{};
	PlayerInputMapper playerInputMapper;
	RenderContext renderContext;

	void ShutdownSystems() noexcept
	{
		if (!isInitialized)
		{
			return;
		}

		// GPU가 커맨드를 끝내기 전에 씬/리소스를 파기하면 ComPtr 해제 시점 충돌이 날 수 있다.
		if (Renderer* renderer{ renderSystem.GetRenderTarget(mainRenderTarget) })
		{
			renderer->WaitForFrames();
		}

		sceneSystem.Release();
		if (mainRenderTarget.IsValid())
		{
			renderSystem.DestroyRenderer(mainRenderTarget);
			mainRenderTarget = {};
		}
		renderSystem.Release();
		windowSystem.Release();

		isInitialized = false;
	}
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

		if (!windowSystem.Initialize(options))
		{
			ShutdownSystems();
			return false;
		}

		windowSystem.GetMainWindow().Show();
	}

	// 렌더러 생성 및 초기화.
	{
		RendererOptions options{};
		options.window = windowSystem.GetMainWindow().GetHandle();
		options.width = windowSystem.GetMainWindow().GetWidth();
		options.height = windowSystem.GetMainWindow().GetHeight();
		options.msaa4xEnable = false;
		options.enableTripleBuffering = false;
		options.vSync = true;
		options.fullscreen = options_.fullscreen;

		if (!renderSystem.Initialize(options))
		{
			ShutdownSystems();
			return false;
		}

		mainRenderTarget = renderSystem.CreateRenderer(windowSystem.GetMainWindow(), options);
		if (!mainRenderTarget.IsValid())
		{
			ShutdownSystems();
			return false;
		}
	}

	InputManager::Reset();
	InputManager::SetScreenSize(windowSystem.GetMainWindow().GetWidth(), windowSystem.GetMainWindow().GetHeight());
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

	isInitialized = true;
	isRunning = false;
	
	return true;
}

int Application::Run()
{
	isRunning = true;
	float fixedUpdateAccumulator{ 0.0f };

	Event event;
	while (isRunning)
	{
		Timer::Tick();
		InputManager::Update();

		while (windowSystem.GetMainWindow().PollEvent(event))
		{
			switch (event.type)
			{
			case Event::Type::WindowResize:
				renderSystem.OnWindowResize(event.windowHandle, event.resize.width, event.resize.height);
				InputManager::SetScreenSize(event.resize.width, event.resize.height);
				break;
			case Event::Type::WindowFullscreenToggle:
				renderSystem.OnWindowFullscreenToggle(event.windowHandle);
				if (Renderer* renderer{ renderSystem.GetRenderTarget(mainRenderTarget) })
				{
					InputManager::SetScreenSize(renderer->GetWidth(), renderer->GetHeight());
				}
				break;
			case Event::Type::WindowClose:
				isRunning = false;
				break;
			default:
				break;
			}

			InputManager::ProcessEvent(event);
		}

		const PlayerInput playerInput{ playerInputMapper.Build() };
		sceneSystem.HandlePlayerInput(playerInput);

		sceneSystem.Update();

		fixedUpdateAccumulator += Timer::GetUnscaledDeltaTime();
		const float fixedStep{ Timer::GetUnscaledFixedDeltaTime() };
		int fixedUpdateCount{ 0 };

		while (fixedStep > 0.0f && fixedUpdateAccumulator >= fixedStep && fixedUpdateCount < MaxFixedUpdatesPerFrame)
		{
			sceneSystem.FixedUpdate();
			fixedUpdateAccumulator -= fixedStep;
			++fixedUpdateCount;
		}

		if (fixedUpdateCount == MaxFixedUpdatesPerFrame)
		{
			fixedUpdateAccumulator = 0.0f;
		}

		Renderer* renderer{ renderSystem.GetRenderTarget(mainRenderTarget) };
		if (nullptr == renderer)
		{
			break;
		}

		renderer->BeginRender();
		renderContext.Clear();
		sceneSystem.Render(renderContext);
		renderer->EndRender();
	}

	ShutdownSystems();

	return 0;
	// return event.quit.quitCode;
}

void Application::Quit()
{
	isRunning = false;
}

Window& Application::GetWindow()
{
	return windowSystem.GetMainWindow();
}

Renderer& Application::GetRenderer()
{
	Renderer* renderer{ renderSystem.GetRenderTarget(mainRenderTarget) };
	assert(nullptr != renderer);
	return *renderer;
}

SceneSystem& Application::GetSceneSystem()
{
	return sceneSystem;
}
