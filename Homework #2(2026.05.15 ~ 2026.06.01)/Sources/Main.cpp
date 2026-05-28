#include "Precompiled.h"

#include "Engine.h"
#include "EngineOptions.h"

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	EngineOptions options{};
	options.title = L"Homework #2(2026.05.15 ~ 2026.06.01)";
	options.x = 0;
	options.y = 0;
	options.width = 800;
	options.height = 600;
	options.fullscreen = false;
	options.resizable = false;
	options.borderless = false;
	options.fixedTime = 60.0f;

	Engine engine;
	if (!engine.Initialize(options))
	{
		::MessageBoxW(nullptr, L"Cannot intialize the engine!", L"Oops!", NULL);
		return -1;
	}

	return engine.Run();
}
