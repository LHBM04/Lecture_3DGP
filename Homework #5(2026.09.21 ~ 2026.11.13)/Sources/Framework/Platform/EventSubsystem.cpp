#include "Precompiled.hpp"
#include "EventSubsystem.hpp"

namespace TUK::Framework
{
	SystemStatus EventSubsystem::OnStartup()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus EventSubsystem::OnPreTick()
	{
		MSG message{};
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				return SystemStatus::SUCCESS;
			}

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus EventSubsystem::OnTick()
	{
		readBuffer.swap(writeBuffer);

		for (const Action<>& action : readBuffer)
		{
			if (action)
			{
				action();
			}
		}

		readBuffer.clear();
		return SystemStatus::CONTINUE;
	}

	SystemStatus EventSubsystem::OnPostTick()
	{
		return SystemStatus::CONTINUE;
	}

	void EventSubsystem::OnShutdown()
	{
		writeBuffer.clear();
		readBuffer.clear();
		for (auto& [type, delegate] : listeners)
		{
			delegate.Clear();
		}
		listeners.clear();
	}
}
