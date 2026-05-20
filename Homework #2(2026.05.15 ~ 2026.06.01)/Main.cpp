#include "Precompiled.h"
#include "WindowSystem.h"
#include "Window.h"
#include "WindowOptions.h"
#include "WindowCloseEvent.h"
#include "WindowResizeEvent.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "RendererOptions.h"
#include "EventQueue.h"
#include "EventDispatcher.h"
#include "SceneManager.h"
#include "Scene_Title.h"

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	if (!WindowSystem::Initialize(hInstance))
	{
		return -1;
	}

	WindowOptions windowOptions;
	windowOptions.title = L"Homework #2";
	windowOptions.width = 1280;
	windowOptions.height = 720;
	windowOptions.hasBorder = true;
	windowOptions.isResizable = true;
	windowOptions.isFullscreen = false;

	std::unique_ptr<Window> mainWindow = WindowSystem::Create(windowOptions);
	if (nullptr == mainWindow)
	{
		WindowSystem::Release();
		return -1;
	}

	if (!RenderSystem::Initialize())
	{
		mainWindow.reset();
		WindowSystem::Release();
		return -1;
	}

	RendererOptions rendererOptions;
	rendererOptions.window = mainWindow->GetHandle();
	rendererOptions.width = windowOptions.width;
	rendererOptions.height = windowOptions.height;
	rendererOptions.clearColor = ColorRGB(0.12f, 0.15f, 0.22f);
	rendererOptions.allowTearing = true;
	rendererOptions.vSync = true;

	std::unique_ptr<Renderer> renderer = RenderSystem::CreateRenderer(rendererOptions);
	if (nullptr == renderer)
	{
		RenderSystem::Shutdown();
		mainWindow.reset();
		WindowSystem::Release();
		return -1;
	}

	SceneManager::LoadScene(std::make_unique<Scene_Title>());

	bool isRunning = true;
	while (isRunning)
	{
		EventQueue eventQueue;
		if (!WindowSystem::PollEvents(eventQueue))
		{
			break;
		}

		EventDispatcher dispatcher{ eventQueue };

		dispatcher.Dispatch<WindowCloseEvent>(
			[&](WindowCloseEvent& e)
			{
				isRunning = false;
				e.SetHandled(true);
			});

		dispatcher.Dispatch<WindowResizeEvent>(
			[&](WindowResizeEvent& e)
			{
				renderer->Resize(e.width, e.height);
				e.SetHandled(true);
			});

		if (!isRunning)
		{
			break;
		}

		renderer->PreRender();
		renderer->Clear();
		SceneManager::Update();
		SceneManager::Render(*renderer);
		renderer->Present();
	}

	SceneManager::UnloadScene();
	renderer.reset();
	RenderSystem::Shutdown();
	mainWindow.reset();
	WindowSystem::Release();

	return 0;
}
