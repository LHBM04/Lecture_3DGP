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

	Framework framework;

	framework.SetOption(L"App.Instance", hInstance);
	framework.SetOption(L"App.Show", nCmdShow);

	framework.SetOption(L"Window.Title", L"Homework #4(2026.05.18 ~ 2026.06.27)");
	framework.SetOption(L"Window.Width", 800U);
	framework.SetOption(L"Window.Height", 600U);
	
	framework.SetOption(L"Renderer.EnableVSync", false);
	
	try
	{
		if (!Framework::GetInstance().Initialize())
		{
			return -1;
		}

		return Framework::GetInstance().Run();
	}
	catch (const std::exception& ex_)
	{
		::MessageBoxW(nullptr, L"Error", L"Oops!", MB_OK | MB_ICONERROR);
		return -1;
	}
}
