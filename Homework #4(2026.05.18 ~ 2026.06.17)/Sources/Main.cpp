#include "Precompiled.h"

#include "Framework/Core/Engine.h"
#include "Framework/Core/Game.h"
#include "Framework/Entities/SceneSubsystem.h"
#include "Framework/Graphics/RenderSubsystem.h"
#include "Framework/Platform/EventSubsystem.h"
#include "Framework/Platform/TimeSubsystem.h"
#include "Framework/Platform/WindowSubsystem.h"

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

	engine.SetOption<std::string>("Window.Title", "Homework #4");
	engine.SetOption("Window.PositionX", CW_USEDEFAULT);
	engine.SetOption("Window.PositionY", CW_USEDEFAULT);
	engine.SetOption("Window.SizeX", 1280);
	engine.SetOption("Window.SizeY", 720);
	engine.SetOption("Window.IsResizable", true);
	engine.SetOption("Window.IsBorderless", false);
	engine.SetOption("Window.IsFullscreen", false);

	engine.SetOption("RenderContext.BufferCount", 2);

	engine.AddSubsystem<TimeSubsystem>();
	engine.AddSubsystem<EventSubsystem>();
	engine.AddSubsystem<WindowSubsystem>();
	engine.AddSubsystem<RenderSubsystem>();

	Game game;
	game.AddSubsystem<SceneSubsystem>();

	return engine.Run(game);
}
