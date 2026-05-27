#include "Precompiled.h"
#include "Engine.h"

#include "Event.h"
#include "InputSystem.h"
#include "Renderer.h"
#include "RenderSystem.h"
#include "TimeSystem.h"
#include "Window.h"
#include "WindowSystem.h"

bool Engine::Initialize(const Options& options_)
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

		Window::Options windowOptions{};
		windowOptions.title = options_.title;
		windowOptions.x = 0;
		windowOptions.y = 0;
		windowOptions.width = options_.width;
		windowOptions.height = options_.height;
		windowOptions.style = style;
		windowOptions.styleEx = WS_EX_APPWINDOW;

		Window* window{ windowSystem->CreateWindow(windowOptions) };
		if (window == nullptr)
		{
			return false;
		}

		window->Show();
	}

	return true;
}

void Engine::Release()
{

}

int Engine::Run()
{
	bool isRunning{ true };
	while (isRunning)
	{
		for (const std::unique_ptr<Window>& window : windowSystem->GetWindows())
		{
			Event event{};
			while (window->PollEvent(event))
			{
				switch (event.type)
				{
				case Event::Type::WindowResize:
					// mainRenderer->Resize(event.resize.width, event.resize.height);
					// inputSystem.SetScreenSize(event.resize.width, event.resize.height);
					break;
				case Event::Type::WindowFullscreenToggle:
					// mainRenderer->ToggleFullscreen();
					// inputSystem.SetScreenSize(mainRenderer->GetWidth(), mainRenderer->GetHeight());
					break;
				case Event::Type::WindowClose:
					isRunning = false;
					break;
				default:
					break;
				}
			}
		}
	}

	return 0;
}
