#include "Precompiled.hpp"
#include "Framework/Platform/EventSubsystem.hpp"

namespace TUK::Framework
{
	EventSubsystem::EventSubsystem() noexcept
		: EngineSubsystem(10)
		, writeBuffer()
		, readBuffer()
		, listeners()
	{
	}

	EventSubsystem::~EventSubsystem() noexcept
	{
		listeners.clear();
		readBuffer.clear();
		writeBuffer.clear();
	}

	void EventSubsystem::PushEvent(std::unique_ptr<Event> event)
	{
		if (event)
		{
			writeBuffer.push_back(std::move(event));
		}
	}

	SystemStatus EventSubsystem::OnPreTick()
	{
		MSG message{};
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				return SystemStatus::Success;
			}

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return SystemStatus::Continue;
	}

	SystemStatus EventSubsystem::OnTick()
	{
		readBuffer.clear();
		std::swap(writeBuffer, readBuffer);

		for (const std::unique_ptr<Event>& event : readBuffer)
		{
			if (!event)
			{
				continue;
			}

			const Event& rawEvent = *event;
			const auto found = listeners.find(typeid(rawEvent));
			if (found != listeners.end())
			{
				for (const Listener& listener : found->second)
				{
					listener(rawEvent);
				}
			}
		}

		return SystemStatus::Continue;
	}

	void EventSubsystem::OnShutdown()
	{
		listeners.clear();
		readBuffer.clear();
		writeBuffer.clear();
	}
}
