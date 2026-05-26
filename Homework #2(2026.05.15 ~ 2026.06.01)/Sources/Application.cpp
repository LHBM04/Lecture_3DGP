#include "Precompiled.h"
#include "Application.h"

#include "InputContext.h"
#include "InputPhase.h"
#include "InputManager.h"
#include "Scene_Test.h"
#include "Scene_Title.h"
#include "SceneManager.h"
#include "SceneSystem.h"
#include "TimeContext.h"
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

	float fixedUpdateAccumulator{ 0.0f };

	void DispatchKeyboardInput(SceneSystem& sceneSystem_)
	{
		const auto dispatchKey =
			[&sceneSystem_](KeyCode key_)
			{
				if (InputManager::IsKeyPressed(key_))
				{
					sceneSystem_.DispatchInput(InputContext{ key_, InputPhase::Started });
				}

				if (InputManager::IsKeyDown(key_))
				{
					sceneSystem_.DispatchInput(InputContext{ key_, InputPhase::Performed });
				}

				if (InputManager::IsKeyReleased(key_))
				{
					sceneSystem_.DispatchInput(InputContext{ key_, InputPhase::Canceled });
				}
			};

		dispatchKey(KeyCode::W);
		dispatchKey(KeyCode::A);
		dispatchKey(KeyCode::S);
		dispatchKey(KeyCode::D);

		dispatchKey(KeyCode::Up);
		dispatchKey(KeyCode::Left);
		dispatchKey(KeyCode::Down);
		dispatchKey(KeyCode::Right);

		dispatchKey(KeyCode::Space);
		dispatchKey(KeyCode::Shift);
		dispatchKey(KeyCode::Escape);
	}

	void ShutdownSystems() noexcept
	{
		if (!isInitialized)
		{
			return;
		}

		if (Renderer* renderer{ renderSystem.GetRenderTarget(mainRenderTarget) })
		{
			renderer->WaitForFrames();
		}

		sceneSystem.Release();
		renderSystem.DestroyRenderer(mainRenderTarget);
		mainRenderTarget = {};
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

		WindowOptions options{};
		options.title = options_.title;
		options.x = 0;
		options.y = 0;
		options.width = options_.width;
		options.height = options_.height;
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
		Window& mainWindow{ windowSystem.GetMainWindow() };

		RendererOptions options{};
		options.window = mainWindow.GetHandle();
		options.width = mainWindow.GetWidth();
		options.height = mainWindow.GetHeight();
		options.msaa4xEnable = false;
		options.enableTripleBuffering = false;
		options.vSync = true;
		options.fullscreen = options_.fullscreen;

		if (!renderSystem.Initialize(options))
		{
			ShutdownSystems();
			return false;
		}

		mainRenderTarget = renderSystem.CreateRenderer(mainWindow, options);
		if (!mainRenderTarget.IsValid())
		{
			ShutdownSystems();
			return false;
		}
	}

	InputManager::Reset();
	Window& mainWindow{ windowSystem.GetMainWindow() };
	InputManager::SetScreenSize(mainWindow.GetWidth(), mainWindow.GetHeight());
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
	fixedUpdateAccumulator = 0.0f;
	Window& mainWindow{ windowSystem.GetMainWindow() };

	Event event;
	while (isRunning)
	{
		Timer::Tick();
		InputManager::Update();

		while (mainWindow.PollEvent(event))
		{
			switch (event.type)
			{
			case Event::Type::WindowResize:
				renderSystem.OnWindowResize(mainWindow.GetHandle(), event.resize.width, event.resize.height);
				InputManager::SetScreenSize(event.resize.width, event.resize.height);
				break;
			case Event::Type::WindowFullscreenToggle:
				renderSystem.OnWindowFullscreenToggle(mainWindow.GetHandle());
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

		TimeContext timeContext{};
		timeContext.timeScale = Timer::GetTimeScale();
		timeContext.deltaTime = Timer::GetDeltaTime();
		timeContext.fixedDeltaTime = Timer::GetFixedDeltaTime();
		timeContext.unscaledDeltaTime = Timer::GetUnscaledDeltaTime();
		timeContext.unscaledFixedDeltaTime = Timer::GetUnscaledFixedDeltaTime();
		timeContext.totalTime = Timer::GetTotalTime();
		timeContext.unscaledTotalTime = Timer::GetUnscaledTime();
		timeContext.fps = Timer::GetFps();
		DispatchKeyboardInput(sceneSystem);

		sceneSystem.Update(timeContext);

		fixedUpdateAccumulator += timeContext.unscaledDeltaTime;
		const float fixedStep{ timeContext.unscaledFixedDeltaTime };
		int fixedUpdateCount{ 0 };

		while (fixedStep > 0.0f && fixedUpdateAccumulator >= fixedStep && fixedUpdateCount < MaxFixedUpdatesPerFrame)
		{
			sceneSystem.FixedUpdate(timeContext);
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
		RenderContext& context{ renderer->GetContext() };
		sceneSystem.Render(context);
		renderer->Flush();
		renderer->EndRender();
	}

	ShutdownSystems();

	return 0;
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
	assert(renderer);
	return *renderer;
}

SceneSystem& Application::GetSceneSystem()
{
	return sceneSystem;
}
