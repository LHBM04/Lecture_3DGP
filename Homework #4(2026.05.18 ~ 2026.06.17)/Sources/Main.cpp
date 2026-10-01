#include "Precompiled.h"

#include "Application.h"
#include "Logger.h"

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if defined(_DEBUG)
	AllocConsole();

	FILE* consoleStream = nullptr;
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);

	LOGTRACE("로그 초기화 완료!");
#endif
	Application::Options options{};
	options.title = TEXT("Homework #4(2026.05.18 ~ 2026.06.17)");
	options.x = CW_USEDEFAULT;
	options.y = CW_USEDEFAULT;
	options.width = 1280;
	options.height = 720;
	options.style = WS_OVERLAPPEDWINDOW;

	return Application::Run(options);
}