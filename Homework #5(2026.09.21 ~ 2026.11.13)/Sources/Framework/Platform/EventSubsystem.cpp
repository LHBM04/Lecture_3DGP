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

		for (const auto& event : readBuffer)
		{
			const auto result = listeners.find(std::type_index(typeid(*event)));
			if (result == listeners.end())
			{
				continue;
			}

			const auto callbacks = result->second;
			for (const EventCallback& callback : callbacks)
			{
				callback(*event);
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
		listeners.clear();
	}
}
