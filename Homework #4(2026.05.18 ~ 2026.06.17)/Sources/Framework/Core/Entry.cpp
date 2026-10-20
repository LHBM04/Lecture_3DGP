#include "Precompiled.hpp"

#include "Engine.hpp"
#include "Game.hpp"
#include "Memory.hpp"

/** 기본 8MB */
std::size_t frameBufferSize = 8 * 1024 * 1024;

/** 기본 32MB */
std::size_t persistentBufferSize = 32 * 1024 * 1024;

/** 종료 코드 */
INT exitCode = 0;

using namespace TUK::Framework;

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);
	
	int argc = 0;
	LPWSTR* const argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	
	if (argc >= 3)
	{
		try
		{
			persistentBufferSize = std::stoull(argv[1]);
			frameBufferSize = std::stoull(argv[2]);
		}
		catch (...)
		{
			// 잘못된 문자열이 들어왔을 때 기본값 유지
		}
	}

	Memory::Initialize(persistentBufferSize, frameBufferSize);
	{
		std::unique_ptr<Engine> engine(CreateEngineInstance());
		std::unique_ptr<Game> game(CreateGameInstance());

		exitCode = engine->Run(*game);
	}
	Memory::Release();

	return exitCode;
}
