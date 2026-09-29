#include "Precompiled.h"
#include "Framework/Platform/Window.h"

#include "Framework/Platform/WindowFlags.h"
#include "Framework/Platform/WindowOptions.h"
#include "Platform/WindowInternal.h"

namespace TUK::Framework
{
	Window* AddWindow(const WindowOptions& options)
	{
		HWND hWnd = CreateWindowExW(
			0,
			L"STATIC",
			options.title.data(),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			options.positionX, options.positionY, options.sizeX, options.sizeY,
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
