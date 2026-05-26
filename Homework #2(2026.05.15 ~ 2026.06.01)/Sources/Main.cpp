#include "Precompiled.h"
#include "Application.h"

#include "Logger.h"

INT APIENTRY WinMain(
	_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ LPSTR,
	_In_ INT)
{
#if defined(_DEBUG)
	if (AllocConsole())
	{
		FILE* pFile{ nullptr };
		freopen_s(&pFile, "CONOUT$", "w", stdout);
		freopen_s(&pFile, "CONOUT$", "w", stderr);

		Logger::Trace("콘솔 창이 생성되었습니다, 로그를 출력합니다...");
		Logger::Info("로그 출력이 정상적으로 이루어지고 있습니다.");
	}
#endif

	ApplicationOptions options{};
	options.title = { L"Homework #2(2026.05.15 ~ 2026.06.01)" };
	options.width = 1280;
	options.height = 720;
	options.fullscreen = false;
	options.borderless = false;
	options.resizable = false;

	Application app{};
	if (!app.Initialize(options))
	{
		return -1;
	}

	return app.Run();
}
