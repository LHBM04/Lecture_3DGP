#include "Precompiled.h"
#include "Application.h"

#include "InputManager.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene_Test.h"

namespace
{
	bool isRunning;

	Window window;
	Renderer renderer;
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

	std::unique_ptr<Scene_Test> scene{ std::make_unique<Scene_Test>() };
	if (!scene->LoadResources(renderer.GetDevice()))
	{
		return false;
	}

	SceneManager::AddScene(L"Test", std::move(scene));
	SceneManager::LoadScene(L"Test");

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
		}

		SceneManager::Update();

		renderer.BeginRender();
		renderer.Clear();

		SceneManager::Render();

		renderer.EndRender();
	}

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

