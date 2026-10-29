#include "Precompiled.hpp"
#include "EventSubsystem.hpp"

#include "../Core/Memory.hpp"

namespace TUK::Framework
{
	EventSubsystem::EventSubsystem() noexcept
		: EngineSubsystem(10)
		, writeBuffer(Memory::GetPersistentResource())
		, readBuffer(Memory::GetPersistentResource())
	{
	}

	EventSubsystem::~EventSubsystem() noexcept = default;

	void EventSubsystem::PushEvent(Event event)
	{
		writeBuffer.push_back(std::move(event));
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

		for (const Event& event : readBuffer)
		{
			for (const Listener& listener : listeners[event.data.index()])
			{
				listener(event);
			}
		}

		return SystemStatus::Continue;
	}

	void EventSubsystem::OnShutdown()
	{
		for (std::vector<Listener>& eventListeners : listeners)
		{
			eventListeners.clear();
		}
		readBuffer.clear();
		writeBuffer.clear();
	}
}
