#include "Precompiled.h"
#include "Application.h"

#include "InputContext.h"
#include "InputPhase.h"
#include "Mesh.h"
#include "Scene_Test.h"
#include "Scene_Title.h"
#include "Shader.h"
#include "TimeContext.h"
#include "Timer.h"

#define SCENE_ENTRY(SceneType, SceneName) \
	SceneBuildEntry{ SceneName, []() -> std::unique_ptr<Scene> { return std::make_unique<SceneType>(); } }

namespace
{
	constexpr int MaxFixedUpdatesPerFrame{ 5 };
}

Application::Application() noexcept
	: isRunning{ false }
	, isInitialized{ false }
	, fixedUpdateAccumulator{ 0.0f }
	, mainRenderTarget{}
{
}

Application::~Application() noexcept
{
	ShutdownSystems();
}

bool Application::Initialize(const ApplicationOptions& options_)
{
	WindowSystem& windowSystem{ AddSystem<WindowSystem>() };
	RenderSystem& renderSystem{ AddSystem<RenderSystem>() };
	SceneSystem& sceneSystem{ AddSystem<SceneSystem>() };
	InputSystem& inputSystem{ AddSystem<InputSystem>() };
	ResourceSystem& resourceSystem{ AddSystem<ResourceSystem>() };

	DWORD style{ WS_OVERLAPPEDWINDOW };
	if (options_.borderless)
	{
		style = WS_POPUP;
	}
	else if (!options_.resizable)
	{
		style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	}

	WindowOptions windowOptions{};
	windowOptions.title = options_.title;
	windowOptions.x = 0;
	windowOptions.y = 0;
	windowOptions.width = options_.width;
	windowOptions.height = options_.height;
	windowOptions.style = style;
	windowOptions.styleEx = WS_EX_APPWINDOW;

	if (!windowSystem.Initialize(windowOptions))
	{
		ShutdownSystems();
		return false;
	}
	windowSystem.GetMainWindow().Show();

	RendererOptions rendererOptions{};
	rendererOptions.window = windowSystem.GetMainWindow().GetHandle();
	rendererOptions.width = windowSystem.GetMainWindow().GetWidth();
	rendererOptions.height = windowSystem.GetMainWindow().GetHeight();
	rendererOptions.enableTripleBuffering = false;
	rendererOptions.vSync = true;
	rendererOptions.fullscreen = options_.fullscreen;

	if (!renderSystem.Initialize())
	{
		ShutdownSystems();
		return false;
	}

	mainRenderTarget = renderSystem.CreateRenderer(windowSystem.GetMainWindow(), rendererOptions);
	if (!mainRenderTarget.IsValid())
	{
		ShutdownSystems();
		return false;
	}

	Renderer* renderer{ renderSystem.GetRenderTarget(mainRenderTarget) };
	if (nullptr == renderer)
	{
		ShutdownSystems();
		return false;
	}
	Mesh::SetDefaultDevice(renderer->GetDevice());
	Shader::SetDefaultDevice(renderer->GetDevice());

	InputOptions inputOptions{};
	inputOptions.screenWidth = windowSystem.GetMainWindow().GetWidth();
	inputOptions.screenHeight = windowSystem.GetMainWindow().GetHeight();
	if (!inputSystem.Initialize(inputOptions))
	{
		ShutdownSystems();
		return false;
	}

	ResourceOptions resourceOptions{};
	if (!resourceSystem.Initialize(resourceOptions))
	{
		ShutdownSystems();
		return false;
	}

	Timer::Reset();

	SceneOptions sceneOptions{
		.scenes =
		{
			SCENE_ENTRY(Scene_Test,  L"Title"),
			SCENE_ENTRY(Scene_Title, L"Game")
		},
		.startIndex = 0
	};
	if (!sceneSystem.Initialize(sceneOptions))
	{
		ShutdownSystems();
		return false;
	}
	sceneSystem.ConfigureContext(&inputSystem, renderer->GetDevice());

	isInitialized = true;
	isRunning = false;
	return true;
}

int Application::Run()
{
	WindowSystem& windowSystem{ GetSystemRef<WindowSystem>() };
	RenderSystem& renderSystem{ GetSystemRef<RenderSystem>() };
	SceneSystem& sceneSystem{ GetSystemRef<SceneSystem>() };
	InputSystem& inputSystem{ GetSystemRef<InputSystem>() };

	isRunning = true;
	fixedUpdateAccumulator = 0.0f;

	Event event{};
	while (isRunning)
	{
		Timer::Tick();
		inputSystem.Update();

		Window& mainWindow{ windowSystem.GetMainWindow() };
		Renderer* mainRenderer{ renderSystem.GetRenderTarget(mainRenderTarget) };
		if (nullptr == mainRenderer)
		{
			break;
		}

		while (mainWindow.PollEvent(event))
		{
			switch (event.type)
			{
			case Event::Type::WindowResize:
				mainRenderer->Resize(event.resize.width, event.resize.height);
				inputSystem.SetScreenSize(event.resize.width, event.resize.height);
				break;
			case Event::Type::WindowFullscreenToggle:
				mainRenderer->ToggleFullscreen();
				inputSystem.SetScreenSize(mainRenderer->GetWidth(), mainRenderer->GetHeight());
				break;
			case Event::Type::WindowClose:
				isRunning = false;
				break;
			default:
				break;
			}

			inputSystem.ProcessEvent(event);
		}

		DispatchKeyboardInput();

		TimeContext timeContext{};
		timeContext.timeScale = Timer::GetTimeScale();
		timeContext.deltaTime = Timer::GetDeltaTime();
		timeContext.fixedDeltaTime = Timer::GetFixedDeltaTime();
		timeContext.unscaledDeltaTime = Timer::GetUnscaledDeltaTime();
		timeContext.unscaledFixedDeltaTime = Timer::GetUnscaledFixedDeltaTime();
		timeContext.totalTime = Timer::GetTotalTime();
		timeContext.unscaledTotalTime = Timer::GetUnscaledTime();
		timeContext.fps = Timer::GetFps();

		sceneSystem.Update(timeContext);

		fixedUpdateAccumulator += timeContext.unscaledDeltaTime;
		const float fixedStep{ timeContext.unscaledFixedDeltaTime };
		int fixedUpdateCount{ 0 };

		while (fixedStep > 0.0f &&
			fixedUpdateAccumulator >= fixedStep &&
			fixedUpdateCount < MaxFixedUpdatesPerFrame)
		{
			sceneSystem.FixedUpdate(timeContext);
			fixedUpdateAccumulator -= fixedStep;
			++fixedUpdateCount;
		}

		if (fixedUpdateCount == MaxFixedUpdatesPerFrame)
		{
			fixedUpdateAccumulator = 0.0f;
		}

		mainRenderer->BeginRender();
		RenderContext& renderContext{ mainRenderer->GetContext() };
		renderContext.Clear();
		sceneSystem.Render(renderContext);
		mainRenderer->Flush();
		mainRenderer->EndRender();
	}

	ShutdownSystems();
	return 0;
}

void Application::Quit() noexcept
{
	isRunning = false;
}

void Application::ShutdownSystems() noexcept
{
	if (!isInitialized)
	{
		return;
	}

	RenderSystem& renderSystem{ GetSystemRef<RenderSystem>() };
	if (Renderer* mainRenderer{ renderSystem.GetRenderTarget(mainRenderTarget) })
	{
		mainRenderer->WaitForFrames();
	}

	ReleaseSystemsInReverseOrder();
	Mesh::SetDefaultDevice(nullptr);
	Shader::SetDefaultDevice(nullptr);

	mainRenderTarget = {};
	isInitialized = false;
	isRunning = false;
}

void Application::DispatchKeyboardInput() noexcept
{
	SceneSystem& sceneSystem{ GetSystemRef<SceneSystem>() };
	InputSystem& inputSystem{ GetSystemRef<InputSystem>() };

	const auto dispatchKey =
		[&sceneSystem, &inputSystem](const KeyCode key_)
		{
			if (inputSystem.IsKeyPressed(key_))
			{
				sceneSystem.DispatchInput(InputContext{ key_, InputPhase::Started });
			}

			if (inputSystem.IsKeyDown(key_))
			{
				sceneSystem.DispatchInput(InputContext{ key_, InputPhase::Performed });
			}

			if (inputSystem.IsKeyReleased(key_))
			{
				sceneSystem.DispatchInput(InputContext{ key_, InputPhase::Canceled });
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

void Application::ReleaseSystemsInReverseOrder() noexcept
{
	for (auto iter{ systemOrder.rbegin() }; iter != systemOrder.rend(); ++iter)
	{
		auto found{ systems.find(*iter) };
		if (found == systems.end() || !found->second)
		{
			continue;
		}

		found->second->Release();
	}

	systems.clear();
	systemOrder.clear();
}
