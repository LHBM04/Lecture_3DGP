#include "Precompiled.hpp"

#include "Framework/Core.hpp"

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
	SetConsoleOutputCP(CP_UTF8);
	
	FILE* pFile{ nullptr };
	freopen_s(&pFile, "CONOUT$", "w", stdout);
	freopen_s(&pFile, "CONOUT$", "w", stderr);

	LOG_INFO("디버그 콘솔이 활성화되었습니다.");
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
