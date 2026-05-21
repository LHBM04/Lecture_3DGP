#include "Precompiled.h"
#include "Application.h"

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ INT nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Application::Options options{};
	options.showCommand = nCmdShow;

	if (!Application::Initialize(hInstance, options))
	{
		::MessageBoxW(nullptr, L"애플리케이션을 생성할 수 없습니다!", L"Oops!", MB_OK | MB_ICONERROR);
		return -1;
	}

	return Application::Run();
}
