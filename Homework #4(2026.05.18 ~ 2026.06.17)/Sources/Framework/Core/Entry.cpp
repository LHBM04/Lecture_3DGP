#include "Precompiled.hpp"

#include "Engine.hpp"
#include "App.hpp"
#include "Memory.hpp"

/** 기본 8MB */
std::size_t frameBufferSize = 8 * 1024 * 1024;

/** 기본 32MB */
std::size_t persistentBufferSize = 32 * 1024 * 1024;

/** 종료 코드 */
INT exitCode = 0;

using namespace TUK::Framework;

INT APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	const int argc = __argc;
	char** const argv = __argv;
	
	if (argc >= 3)
	{
		persistentBufferSize = std::stoull(argv[1]);
		frameBufferSize = std::stoull(argv[2]);
	}

	Memory::Initialize(persistentBufferSize, frameBufferSize);
	{
		std::unique_ptr<Engine> engine(CreateEngineInstance());
		std::unique_ptr<App> app(CreateAppInstance());

		exitCode = engine->Run(*app);
	}
	Memory::Release();

	return exitCode;
}
