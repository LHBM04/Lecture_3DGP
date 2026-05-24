#include "Precompiled.h"

#include "Input.h"
#include "RendererOptions.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Timer.h"
#include "Window.h"
#include "WindowOptions.h"

#include "Scene_Test.h"

void ReleaseResources(Window& window_)
{
	Renderer::GetInstance().WaitIdle();
	SceneManager::GetInstance().Release();
	ResourceManager::GetInstance().Clear();
	Input::GetInstance().Release();
	Renderer::GetInstance().Release();
	window_.Release();
}

INT APIENTRY wWinMain(
	_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ PWSTR,
	_In_ INT)
{
	const std::wstring title{ L"Homework #2(2026.05.15 ~ 2026.06.01)" };
	const int width{ 1280 };
	const int height{ 720 };
	const bool fullscreen{ false };
	const bool resizable{ true };
	const bool borderless{ false };

	Window window{};

	WindowOptions windowOptions;
	windowOptions.title = title;
	windowOptions.x = CW_USEDEFAULT;
	windowOptions.y = CW_USEDEFAULT;
	windowOptions.width = width;
	windowOptions.height = height;
	windowOptions.style = WS_OVERLAPPEDWINDOW;
	windowOptions.styleEx = WS_EX_APPWINDOW;

	if (!resizable)
	{
		windowOptions.style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	}

	if (borderless || fullscreen)
	{
		windowOptions.style = WS_POPUP;
	}

	if (!window.Initialize(windowOptions))
	{
		return -1;
	}

	window.Show();

	RendererOptions rendererOptions;
	rendererOptions.window = window.GetHandle();
	rendererOptions.x = 0;
	rendererOptions.y = 0;
	rendererOptions.width = width;
	rendererOptions.height = height;
	rendererOptions.msaa4xEnable = false;
	rendererOptions.enableTripleBuffering = false;
	rendererOptions.vSync = true;

	if (!Renderer::GetInstance().Initialize(rendererOptions))
	{
		ReleaseResources(window);
		return -1;
	}

	if (!Input::GetInstance().Initialize())
	{
		ReleaseResources(window);
		return -1;
	}

	Timer::GetInstance().Initialize();

	Input::GetInstance().SetScreenSize(width, height);

	std::unique_ptr<Scene_Test> testScene{ std::make_unique<Scene_Test>() };
	if (!testScene->LoadResources(Renderer::GetInstance().GetDevice()))
	{
		ReleaseResources(window);
		return -1;
	}

	SceneManager::GetInstance().ChangeScene(std::move(testScene));
	if (!SceneManager::GetInstance().Initialize())
	{
		ReleaseResources(window);
		return -1;
	}

	bool isRunning{ true };

	while (isRunning)
	{
		Timer::GetInstance().Tick();
		Input::GetInstance().Update();

		Event event{};
		while (window.PollEvent(event))
		{
			switch (event.type)
			{
			case Event::Type::WindowResize:
				Input::GetInstance().SetScreenSize(event.resize.width, event.resize.height);
				break;
			case Event::Type::WindowClose:
				isRunning = false;
				break;
			default:
				break;
			}

			Input::GetInstance().ProcessEvent(event);
		}

		SceneManager::GetInstance().Update();

		Renderer::GetInstance().BeginRender();
		Renderer::GetInstance().Clear();
		
		SceneManager::GetInstance().Render();

		// Renderer::GetInstance().Present();
		Renderer::GetInstance().EndRender();
	}

	ReleaseResources(window);
	return 0;
}
