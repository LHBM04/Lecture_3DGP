#include "Precompiled.h"

INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ PWSTR lpCmdLine, 
	_In_ INT nCmdShow)
{
	::MessageBoxW(nullptr, L"Hello, World!", L"Notice", NULL);
	return 0;
}