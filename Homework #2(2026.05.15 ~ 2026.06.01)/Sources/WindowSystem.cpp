#include "Precompiled.h"
#include "WindowSystem.h"

void WindowSystem::PollEvents(EventQueue& eventQueue_)
{
	for (std::unique_ptr<Window>& window : windows)
	{
		if (window != nullptr)
		{
			window->eventQueue = eventQueue_;
		}
	}

	MSG message{};
	while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}

	for (const std::unique_ptr<Window>& window : windows)
	{
		if (window != nullptr)
		{
			window->currentEventQueue = nullptr;
		}
	}
}