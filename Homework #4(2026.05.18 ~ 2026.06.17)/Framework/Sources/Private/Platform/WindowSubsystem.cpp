#include "Precompiled.h"
#include "Framework/Platform/WindowSubsystem.h"

#include "Framework/Platform/Window.h"
#include "Framework/Platform/WindowFlags.h"
#include "Framework/Platform/WindowOptions.h"

namespace TUK::Framework
{
	Window* WindowSubsystem::AddWindow(const WindowOptions& options)
	{
		return ::TUK::Framework::AddWindow(options);
	}

	void WindowSubsystem::RemoveWindow(Window* window)
	{
		
	}
}
