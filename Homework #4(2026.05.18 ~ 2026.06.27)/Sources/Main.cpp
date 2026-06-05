#include "Precompiled.h"
#include "Framework.h"

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Framework& framework{ Framework::GetInstance() };

	framework.SetOption<HINSTANCE>(L"App.Instance", hInstance);
	framework.SetOption<int>(L"App.Show", nCmdShow);

	framework.SetOption<LPCWSTR>(L"Window.Title", L"Homework #4(2026.05.18 ~ 2026.06.27)");
	framework.SetOption<int>(L"Window.Width", 800);
	framework.SetOption<int>(L"Window.Height", 600);
	
	framework.SetOption<bool>(L"Renderer.EnableVSync", false);
	
	if (!framework.Initialize())
	{
		return -1;
	}

	return framework.Run();
}
