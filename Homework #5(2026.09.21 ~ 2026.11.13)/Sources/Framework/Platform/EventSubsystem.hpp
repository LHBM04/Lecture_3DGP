#pragma once

#include <concepts>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../Core/Common.hpp"
#include "../Core/EngineSubsystem.hpp"

#include "Event.hpp"

namespace TUK::Framework
{
	class EventSubsystem : public EngineSubsystem
	{
	public:
		explicit EventSubsystem() noexcept;

		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

		template <std::derived_from<Event> TEvent>
		void PushEvent(const TEvent& event);

		template <std::derived_from<Event> TEvent, class... TArgs>
		void PushEvent(TArgs&&... args);

		template <std::derived_from<Event> TEvent>
		void DispatchEvent(const TEvent& event);

		template <std::derived_from<Event> TEvent, class... TArgs>
		void DispatchEvent(TArgs&&... args);

		template <std::derived_from<Event> TEvent>
		Delegate<const Event&>::ListenerId Subscribe(Action<const TEvent&> callback);

		template <std::derived_from<Event> TEvent, class TObject>
		Delegate<const Event&>::ListenerId Subscribe(TObject* object, void (TObject::*method)(const TEvent&));

		template <std::derived_from<Event> TEvent>
		void Unsubscribe(Delegate<const Event&>::ListenerId id) noexcept;

	private:
		std::vector<Action<>> writeBuffer;
		std::vector<Action<>> readBuffer;
		std::unordered_map<std::type_index, Delegate<const Event&>> listeners;
	};

	inline EventSubsystem::EventSubsystem() noexcept
		: EngineSubsystem(0)
	{
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::PushEvent(const TEvent& event)
	{
		writeBuffer.push_back([this, event]()
		{
			DispatchEvent(event);
		});
	}

	template <std::derived_from<Event> TEvent, class... TArgs>
	void EventSubsystem::PushEvent(TArgs&&... args)
	{
		writeBuffer.push_back([this, storedEvent = TEvent(std::forward<TArgs>(args)...)]()
		{
			DispatchEvent(storedEvent);
		});
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::DispatchEvent(const TEvent& event)
	{
		const auto result = listeners.find(std::type_index(typeid(TEvent)));
		if (result == listeners.end())
		{
			return;
		}

		result->second.Broadcast(event);
	}

	template <std::derived_from<Event> TEvent, class... TArgs>
	void EventSubsystem::DispatchEvent(TArgs&&... args)
	{
		const auto result = listeners.find(std::type_index(typeid(TEvent)));
		if (result == listeners.end())
		{
			return;
		}

		const TEvent event(std::forward<TArgs>(args)...);
		result->second.Broadcast(event);
	}

	template <std::derived_from<Event> TEvent>
	Delegate<const Event&>::ListenerId EventSubsystem::Subscribe(Action<const TEvent&> callback)
	{
		return listeners[std::type_index(typeid(TEvent))].AddListener(
			[callback = std::move(callback)](const Event& event)
			{
				if (callback)
				{
					callback(static_cast<const TEvent&>(event));
				}
			});
	}

	template <std::derived_from<Event> TEvent, class TObject>
	Delegate<const Event&>::ListenerId EventSubsystem::Subscribe(TObject* object, void (TObject::*method)(const TEvent&))
	{
		return Subscribe<TEvent>(std::bind_front(method, object));
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::Unsubscribe(Delegate<const Event&>::ListenerId id) noexcept
	{
		const auto result = listeners.find(std::type_index(typeid(TEvent)));
		if (result != listeners.end())
		{
			result->second.RemoveListener(id);
		}
	}
}
