#include "Precompiled.h"
#include "Framework.h"

#include "RenderService.h"
#include "Window.h"
#include "WindowService.h"

bool Framework::Initialize() noexcept
{
	WindowService& windowService{ AddService<WindowService>() };

	WindowOptions options{};
	options.hInstance = GetOption<HINSTANCE>(L"App.Instance");
	options.title = GetOption<LPCWSTR>(L"Window.Title");
	options.x = 0;
	options.y = 0;
	options.width = GetOption<int>(L"Window.Width");
	options.height = GetOption<int>(L"Window.Height");
	options.style = WS_OVERLAPPEDWINDOW;
	options.styleEx = 0;

	Window* mainWindow{ windowService.AddWindow(options) };
	if (mainWindow == nullptr)
	{
		return false;
	}

	windowService.SetMainWindow(*mainWindow);

	RenderService& renderService{ AddService<RenderService>() };
	if (!renderService.IsInitialized())
	{
		return false;
	}

	return true;
}

int Framework::Run() noexcept
{
	bool isRunning{ true };
	while (isRunning)
	{
		if (!GetService<WindowService>().PollEvents())
		{
			isRunning = false;
			continue;
		}

		RenderService& renderService{ GetService<RenderService>() };
		renderService.BeginFrame();
		renderService.Clear();
		renderService.Present();
	}

	Shutdown();
	return 0;
}

void Framework::Shutdown() noexcept
{
	if (services.contains(typeid(RenderService)))
	{
		services[typeid(RenderService)]->NotifyRemove();
		services.erase(typeid(RenderService));
	}

	if (services.contains(typeid(WindowService)))
	{
		services[typeid(WindowService)]->NotifyRemove();
		services.erase(typeid(WindowService));
	}
}
