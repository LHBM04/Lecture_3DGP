#pragma once

#include <concepts>
#include <memory>
#include <typeindex>
#include <type_traits>
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
		void PushEvent(TEvent&& event);

		template <std::derived_from<Event> TEvent, class... TArgs>
		void PushEvent(TArgs&&... args);

		template <std::derived_from<Event> TEvent>
		void DispatchEvent(TEvent&& event);

		template <std::derived_from<Event> TEvent, class... TArgs>
		void DispatchEvent(TArgs&&... args);

		template <std::derived_from<Event> TEvent>
		[[nodiscard]] Delegate<const TEvent&>& OnEvent() noexcept;

	private:
		using EventDeleter = void (*)(const Event*) noexcept;

		struct EventChannel
		{
			virtual ~EventChannel() noexcept = default;
			virtual void Broadcast(const Event& event) const = 0;
			virtual void Clear() noexcept = 0;
		};

		template <std::derived_from<Event> TEvent>
		struct TypedEventChannel final : EventChannel
		{
			void Broadcast(const Event& event) const override;
			void Clear() noexcept override;

			Delegate<const TEvent&> onEvent;
		};

		struct QueuedEvent
		{
			std::type_index type;
			std::unique_ptr<const Event, EventDeleter> event;
		};

		std::vector<QueuedEvent> writeBuffer;
		std::vector<QueuedEvent> readBuffer;
		std::unordered_map<std::type_index, std::unique_ptr<EventChannel>> onEvents;
	};

	inline EventSubsystem::EventSubsystem() noexcept
		: EngineSubsystem(0)
	{
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::PushEvent(TEvent&& event)
	{
		using EventType = std::remove_cvref_t<TEvent>;
		std::unique_ptr<const Event, EventDeleter> storedEvent(
			new const EventType(std::forward<TEvent>(event)),
			[](const Event* value) noexcept
			{
				delete static_cast<const EventType*>(value);
			});
		writeBuffer.push_back(QueuedEvent{ std::type_index(typeid(EventType)), std::move(storedEvent) });
	}

	template <std::derived_from<Event> TEvent, class... TArgs>
	void EventSubsystem::PushEvent(TArgs&&... args)
	{
		std::unique_ptr<const Event, EventDeleter> storedEvent(
			new const TEvent(std::forward<TArgs>(args)...),
			[](const Event* value) noexcept
			{
				delete static_cast<const TEvent*>(value);
			});
		writeBuffer.push_back(QueuedEvent{ std::type_index(typeid(TEvent)), std::move(storedEvent) });
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::DispatchEvent(TEvent&& event)
	{
		using EventType = std::remove_cvref_t<TEvent>;
		const auto result = onEvents.find(std::type_index(typeid(EventType)));
		if (result == onEvents.end())
		{
			return;
		}

		result->second->Broadcast(event);
	}

	template <std::derived_from<Event> TEvent, class... TArgs>
	void EventSubsystem::DispatchEvent(TArgs&&... args)
	{
		const auto result = onEvents.find(std::type_index(typeid(TEvent)));
		if (result == onEvents.end())
		{
			return;
		}

		const TEvent event(std::forward<TArgs>(args)...);
		result->second->Broadcast(event);
	}

	template <std::derived_from<Event> TEvent>
	Delegate<const TEvent&>& EventSubsystem::OnEvent() noexcept
	{
		const std::type_index type(typeid(TEvent));
		auto result = onEvents.find(type);
		if (result == onEvents.end())
		{
			result = onEvents.emplace(type, std::make_unique<TypedEventChannel<TEvent>>()).first;
		}
		return static_cast<TypedEventChannel<TEvent>&>(*result->second).onEvent;
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::TypedEventChannel<TEvent>::Broadcast(const Event& event) const
	{
		onEvent.Broadcast(static_cast<const TEvent&>(event));
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::TypedEventChannel<TEvent>::Clear() noexcept
	{
		onEvent.Clear();
	}
}
