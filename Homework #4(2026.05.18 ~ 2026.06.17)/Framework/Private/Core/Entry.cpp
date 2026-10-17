#include "Precompiled.hpp"

#include "Framework/Core/Engine.hpp"
#include "Framework/Core/Game.hpp"

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
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

	std::unique_ptr<Engine> engine = std::unique_ptr<Engine>(CreateEngineInstance());
	std::unique_ptr<Game> game = std::unique_ptr<Game>(CreateGameInstance());

	return engine->Run(*game);
}
