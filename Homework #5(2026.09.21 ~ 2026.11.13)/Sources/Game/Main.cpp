#include "Precompiled.hpp"

#include "Framework/Core.hpp"
#include "Framework/Utilities.hpp"

#include "MyEngine.hpp"
#include "MyGame.hpp"

using namespace TUK::Framework;
using namespace TUK::Game;

INT APIENTRY _tWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ INT nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
	
	FILE* pFile{ nullptr };
	freopen_s(&pFile, "CONOUT$", "w", stdout);
	freopen_s(&pFile, "CONOUT$", "w", stderr);

	Logger::LogInfo("Debug console enabled.");
#endif

	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);

	UNREFERENCED_PARAMETER(lpCmdLine);

	i32 exitCode = 0;
	{
		MyGame app;
		MyEngine engine;
		exitCode = engine.Run(&app);
	}

	return exitCode;
}
