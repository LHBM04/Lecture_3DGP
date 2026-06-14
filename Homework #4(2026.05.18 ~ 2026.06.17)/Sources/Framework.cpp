#include "Precompiled.h"
#include "Framework.h"

#include "RenderService.h"
#include "WindowService.h"

bool Framework::Initialize()
{
	WindowService* windowService{ AddService<WindowService>() };
	if (windowService == nullptr)
	{
		return false;
	}

	WindowService::Options windowOptions{};
	windowOptions.title = GetOption("Window.Title", "New Window");
	windowOptions.width = GetOption("Window.Width", 800);
	windowOptions.height = GetOption("Window.Height", 600);
	windowOptions.isFullscreen = false;
	windowOptions.isResizable = true;
	windowOptions.isBorderless = false;

	windowService->Initialize(windowOptions);

	RenderService* renderService{ AddService<RenderService>() };
	if (renderService == nullptr)
	{
		return false;
	}

	RenderService::Options renderOptions{};
	renderOptions.hWnd = windowService->GetHWND();
	renderOptions.x = 0;
	renderOptions.y = 0;
	renderOptions.width = GetOption("Window.Width", 800);
	renderOptions.height = GetOption("Window.Height", 800);
	renderOptions.isEnableMSAAx4 = true;
	renderOptions.bufferCount = 2;

	renderService->Initialize(renderOptions);
}

int Framework::Run()
{
	MSG msg;
	while (true)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return static_cast<INT>(msg.wParam);
}

void Framework::Quit(int exitCode_)
{
	::PostQuitMessage(exitCode_);
}
