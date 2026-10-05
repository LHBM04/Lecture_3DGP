#include "Precompiled.h"

#include "Framework/Core/System.h"

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

	System system;
	WindowOptions windowOptions;
	windowOptions.title = L"Homework #4";
	const auto option = system.AddOption<WindowOptions>("Window.Options", std::move(windowOptions));
	if (!option)
	{
		OutputDebugStringA(option.error().c_str());
		return EXIT_FAILURE;
	}

	if (const auto renderOption = system.AddOption<UINT>("RenderContext.BufferCount", 2); !renderOption)
	{
		OutputDebugStringA(renderOption.error().c_str());
		return EXIT_FAILURE;
	}

	if (const auto time = system.AddSubsystem<TimeSubsystem>(); !time)
	{
		OutputDebugStringA(time.error().c_str());
		return EXIT_FAILURE;
	}

	if (const auto event = system.AddSubsystem<EventSubsystem>(); !event)
	{
		OutputDebugStringA(event.error().c_str());
		return EXIT_FAILURE;
	}

	if (const auto window = system.AddSubsystem<WindowSubsystem>(); !window)
	{
		OutputDebugStringA(window.error().c_str());
		return EXIT_FAILURE;
	}
	if (const auto render = system.AddSubsystem<RenderSubsystem>(); !render)
	{
		OutputDebugStringA(render.error().c_str());
		return EXIT_FAILURE;
	}

	return system.Run();
}
