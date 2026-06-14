#include "Precompiled.h"
#include "Framework.h"

INT APIENTRY WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, 
	_In_ INT nCmdShow)
{
#ifdef _DEBUG
	assert(::AllocConsole());

	FILE* consoleStream{ nullptr };
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);

	Debugger::LogInfo("Console output initialized.");
#endif

	try
	{
		Framework framework;

		// 창 설정
		framework.SetOption("Window.Title", "Homework #4(2026.05.18 ~ 2026.06.17)");
		framework.SetOption("Window.Width", 1280);
		framework.SetOption("Window.Height", 720);

		if (!framework.Initialize())
		{
			::MessageBox(nullptr, "프로그램이 망했어요", "Oops!", NULL);
			return -1;
		}

		return framework.Run();
	}
	catch (std::exception ex)
	{
		::MessageBox(nullptr, "프로그램이 망했어요", "Oops!", NULL);
		return -1;
	}
}
