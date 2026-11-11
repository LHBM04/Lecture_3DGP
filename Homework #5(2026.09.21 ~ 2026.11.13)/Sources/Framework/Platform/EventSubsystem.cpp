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

		for (const QueuedEvent& queuedEvent : readBuffer)
		{
			const auto result = onEvents.find(queuedEvent.type);
			if (result == onEvents.end())
			{
				continue;
			}

			result->second->Broadcast(*queuedEvent.event);
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
		for (const auto& [type, channel] : onEvents)
		{
			channel->Clear();
		}
		onEvents.clear();
	}
}
