#include "Precompiled.h"

#include "WindowService.h"

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ PWSTR lpCmdLine, 
	_In_ INT nCmdShow)
{
	WindowService windowService;
	windowService.NotifyAdd(nullptr);

	WindowService::Options options{};
	options.title = L"New Window";
	options.width = 800;
	options.height = 600;

	windowService.Initialize(options);

	MSG msg;
	while (true)
	{
		if (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
	}

	return static_cast<INT>(msg.wParam);
}
