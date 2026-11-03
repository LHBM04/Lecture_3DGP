#include "Precompiled.hpp"

#include "MyEngine.hpp"
#include "MyGame.hpp"

using namespace TUK::Game;

static MyGame app;
static MyEngine engine(app);

INT APIENTRY _tWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	return engine.Run();
}
