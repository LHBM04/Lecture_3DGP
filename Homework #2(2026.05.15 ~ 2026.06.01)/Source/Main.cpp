#include "Precompiled.h"
#include "Application.h"

INT APIENTRY WinMain(
	_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ LPSTR,
	_In_ INT)
{
	ApplicationOptions options{};
	options.title = { L"Homework #2(2026.05.15 ~ 2026.06.01)" };
	options.width = 1280;
	options.height = 720;
	options.fullscreen = true;
	options.borderless = false;
	options.resizable = false;

	if (!Application::Initialize(options))
	{
		return -1;
	}

	return Application::Run();
}
