#include "Precompiled.h"

#include "EventDispatcher.h"
#include "EventQueue.h"
#include "Input.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "RenderTargetOptions.h"
#include "Scene_Title.h"
#include "SceneManager.h"
#include "Timer.h"
#include "Window.h"
#include "WindowCloseEvent.h"
#include "WindowOptions.h"
#include "WindowResizeEvent.h"
#include "WindowSystem.h"

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

	if (!Renderer::Initialize())
	{
		mainWindow.reset();
		WindowSystem::Release();
		return -1;
	}

	RenderTargetOptions renderTargetOptions;
	renderTargetOptions.window = mainWindow->GetHandle();
	renderTargetOptions.width = windowOptions.width;
	renderTargetOptions.height = windowOptions.height;
	renderTargetOptions.clearColor = ColorRGB(0.12f, 0.15f, 0.22f);
	renderTargetOptions.allowTearing = true;
	renderTargetOptions.vSync = true;
	Input::SetScreenSize(renderTargetOptions.width, renderTargetOptions.height);

	std::unique_ptr<RenderTarget> renderTarget = Renderer::CreateRenderTarget(renderTargetOptions);
	if (nullptr == renderTarget)
	{
		Renderer::Shutdown();
		mainWindow.reset();
		WindowSystem::Release();
		return -1;
	}

	SceneManager::LoadScene(std::make_unique<Scene_Title>());

	Timer::Initialize();

	bool isRunning = true;
	while (isRunning)
	{
		Input::BeginFrame();

		EventQueue eventQueue;
		if (!WindowSystem::PollEvents(eventQueue))
		{
			break;
		}

		EventDispatcher dispatcher{ eventQueue };

		dispatcher.Dispatch<KeyDownEvent>(
			[](KeyDownEvent& e)
			{
				Input::SetKey(e.keyCode, true);
				e.SetHandled(true);
			});

		dispatcher.Dispatch<KeyUpEvent>(
			[](KeyUpEvent& e)
			{
				Input::SetKey(e.keyCode, false);
				e.SetHandled(true);
			});

		dispatcher.Dispatch<MouseMoveEvent>(
			[](MouseMoveEvent& e)
			{
				Input::SetMousePosition(e.x, e.y);
				e.SetHandled(true);
			});

		dispatcher.Dispatch<MouseButtonDownEvent>(
			[](MouseButtonDownEvent& e)
			{
				Input::SetMousePosition(e.x, e.y);
				Input::SetMouseButton(e.button, true);
				e.SetHandled(true);
			});

		dispatcher.Dispatch<MouseButtonUpEvent>(
			[](MouseButtonUpEvent& e)
			{
				Input::SetMousePosition(e.x, e.y);
				Input::SetMouseButton(e.button, false);
				e.SetHandled(true);
			});

		dispatcher.Dispatch<WindowCloseEvent>(
			[&](WindowCloseEvent& e)
			{
				isRunning = false;
				e.SetHandled(true);
			});

		dispatcher.Dispatch<WindowResizeEvent>(
			[&](WindowResizeEvent& e)
			{
				renderTarget->Resize(e.width, e.height);
				Input::SetScreenSize(e.width, e.height);
				e.SetHandled(true);
			});

		if (!isRunning)
		{
			break;
		}

		renderTarget->PreRender();
		renderTarget->Clear();
		
		Timer::Tick();
		SceneManager::Update();
		
		SceneManager::Render(*renderTarget);
		renderTarget->Present();
	}

	SceneManager::UnloadScene();
	renderTarget.reset();
	Renderer::Shutdown();
	mainWindow.reset();
	WindowSystem::Release();

	return 0;
}
