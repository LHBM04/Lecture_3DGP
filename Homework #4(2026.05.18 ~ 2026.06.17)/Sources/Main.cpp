#include "Precompiled.h"

#include "Framework/Core/Engine.h"
#include "Framework/Core/Game.h"
#include "Framework/Entities/SceneSubsystem.h"

#include "Framework/Platform/WindowSubsystem.h"
#include "Framework/Platform/TimeSubsystem.h"
#include "Framework/Platform/EventSubsystem.h"
#include "Framework/Graphics/RenderSubsystem.h"

using namespace TUK::Framework;

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	AllocConsole();
	FILE* consoleStream = nullptr;
	freopen_s(&consoleStream, "CONOUT$", "w", stdout);
	freopen_s(&consoleStream, "CONOUT$", "w", stderr);
#endif

	Engine engine;

	WindowOptions windowOptions;
	windowOptions.title = L"Homework #4";
	
	engine.AddOption<WindowOptions>("Window.Options", windowOptions);
	engine.AddOption<UINT>("RenderContext.BufferCount", 2);

	engine.AddSubsystem<TimeSubsystem>();
	engine.AddSubsystem<EventSubsystem>();
	engine.AddSubsystem<WindowSubsystem>();
	engine.AddSubsystem<RenderSubsystem>();

	Game game;
	game.AddSubsystem<SceneSubsystem>();

	return engine.Run(game);
}
