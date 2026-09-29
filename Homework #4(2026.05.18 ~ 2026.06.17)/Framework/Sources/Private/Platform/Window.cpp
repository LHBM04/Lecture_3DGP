#include "Precompiled.h"
#include "Framework/Platform/Window.h"

#include "Platform/WindowInternal.h"

namespace TUK::Framework
{
	Window* AddWindow()
	{
		HWND hWnd = CreateWindowExW(
			0,
			L"STATIC",
			L"New Window",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			nullptr
		);
		if (!hWnd)
		{
			throw std::runtime_error("Failed to create window.");
		}
		return new WindowInternal(hWnd);
	}
}
