#include "Precompiled.h"
#include "Engine.h"

#include "EventDispatcher.h"
#include "InputSystem.h"
#include "Renderer.h"
#include "RenderSystem.h"
#include "RendererOptions.h"
#include "TimeSystem.h"
#include "Window.h"
#include "WindowCloseEvent.h"
#include "WindowMaximizeEvent.h"
#include "WindowMinimizeEvent.h"
#include "WindowMoveEvent.h"
#include "WindowResizeEvent.h"
#include "WindowSystem.h"

bool Engine::Initialize(const EngineOptions& options_)
{
	// 창 시스템 초기화 및 창 생성.
	{
		windowSystem = static_cast<WindowSystem*>(AddSystem<WindowSystem>());
		if (!windowSystem->Initialize())
		{
			return false;
		}

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

		auto window{ windowSystem->CreateWindow(options) };
		if (!window)
		{
			return false;
		}

		window.value()->Show();
	}
	// 렌더 시스템 초기화.
	{
		renderSystem = static_cast<RenderSystem*>(AddSystem<RenderSystem>());
		if (!renderSystem->Initialize())
		{
			return false;
		}

		RendererOptions rendererOptions{};
	}

	return true;
}

void Engine::Release()
{

}

int Engine::Run()
{
	isRunning = true;

	while (isRunning)
	{
		eventQueue.Clear();

		windowSystem->PollEvents(eventQueue);

		for (const std::unique_ptr<Event>& event : eventQueue.GetEvents())
		{
			if (event == nullptr)
			{
				continue;
			}

			OnEvent(*event);
		}

		

		// TODO:
		// timeSystem->Update();
		// inputSystem->Update();
		// sceneSystem->Update();
		// renderSystem->Render();
	}

	return 0;
}

bool Engine::OnEvent(Event& event)
{
	EventDispatcher dispatcher{ event };

	if (dispatcher.Dispatch<WindowCloseEvent>(
		[this](WindowCloseEvent& closeEvent) -> bool
		{
			return OnWindowClose(closeEvent);
		}))
	{
		return true;
	}

	if (dispatcher.Dispatch<WindowResizeEvent>(
		[this](WindowResizeEvent& resizeEvent) -> bool
		{
			return OnWindowResize(resizeEvent);
		}))
	{
		return true;
	}

	if (dispatcher.Dispatch<WindowMoveEvent>(
		[this](WindowMoveEvent& moveEvent) -> bool
		{
			return OnWindowMove(moveEvent);
		}))
	{
		return true;
	}

	if (dispatcher.Dispatch<WindowMinimizeEvent>(
		[this](WindowMinimizeEvent& minimizeEvent) -> bool
		{
			return OnWindowMinimize(minimizeEvent);
		}))
	{
		return true;
	}

	if (dispatcher.Dispatch<WindowMaximizeEvent>(
		[this](WindowMaximizeEvent& maximizeEvent) -> bool
		{
			return OnWindowMaximize(maximizeEvent);
		}))
	{
		return true;
	}

	return false;
}

bool Engine::OnWindowClose(WindowCloseEvent& event)
{
	Window* window = event.GetWindow();
	(void)window;

	// 현재는 단일 창 기준으로 엔진 종료.
	// 다중 창 정책이 들어오면 window만 닫고,
	// 남은 창이 없을 때 isRunning = false 처리하도록 확장한다.
	isRunning = false;
	return true;
}

bool Engine::OnWindowResize(WindowResizeEvent& event)
{
	Window* window = event.GetWindow();

	if (window == nullptr)
	{
		return false;
	}

	if (event.width <= 0 || event.height <= 0)
	{
		return false;
	}

	// TODO:
	// 해당 window에 대응되는 Renderer 또는 SwapChain만 Resize해야 한다.
	// renderSystem->Resize(window, event.width, event.height);
	// inputSystem->SetScreenSize(window, event.width, event.height);

	return true;
}

bool Engine::OnWindowMove(WindowMoveEvent& event)
{
	Window* window = event.GetWindow();
	if (window == nullptr)
	{
		return false;
	}

	(void)event.x;
	(void)event.y;
	return true;
}

bool Engine::OnWindowMinimize(WindowMinimizeEvent& event)
{
	Window* window = event.GetWindow();
	if (window == nullptr)
	{
		return false;
	}

	return true;
}

bool Engine::OnWindowMaximize(WindowMaximizeEvent& event)
{
	Window* window = event.GetWindow();
	if (window == nullptr)
	{
		return false;
	}

	return true;
}
