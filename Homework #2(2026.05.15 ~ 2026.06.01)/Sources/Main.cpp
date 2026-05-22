#include "Precompiled.h"
#include "Application.h"

std::unique_ptr<Engine> engine{ nullptr };

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	engine = std::make_unique<Engine>(hInstance);

	Engine::Options options{};
	options.title = L"Homework #2(2026.05.15 ~ 2026.06.01)";
	options.width = 1280;
	options.height = 720;
	options.fullscreen = false;
	options.resizable = true;
	options.borderless = false;

	if (!engine->Initialize(options))
	{
		return -1;
	}

	return engine->Run();
}
