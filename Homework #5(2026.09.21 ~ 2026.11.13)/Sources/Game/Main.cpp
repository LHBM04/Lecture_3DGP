#include "Precompiled.hpp"

#include "../Framework/Core/App.hpp"
#include "../Framework/Core/Engine.hpp"

using namespace TUK::Framework;

INT APIENTRY _tWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	App app;
	Engine engine(app);

	return engine.Run();
}
