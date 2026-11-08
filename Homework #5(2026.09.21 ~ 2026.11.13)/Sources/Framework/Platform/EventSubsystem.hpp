#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../Core/Common.hpp"
#include "../Core/EngineSubsystem.hpp"

#include "../Utilities/Logger.hpp"

#include "Event.hpp"

namespace TUK::Framework
{
	class EventSubsystem : public EngineSubsystem
	{
	public:
		using EventCallback = std::function<void(const Event&)>;

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
		void AddListener(std::function<void(const TEvent&)> callback);

		template <std::derived_from<Event> TEvent, class TInstance, class TArg>
			requires std::invocable<void (TInstance::*)(TArg), TInstance*, const TEvent&>
		void AddListener(TInstance* instance, void (TInstance::* callback)(TArg));

		template <std::derived_from<Event> TEvent, class TInstance, class TArg>
			requires std::invocable<void (TInstance::*)(TArg) const, const TInstance*, const TEvent&>
		void AddListener(const TInstance* instance, void (TInstance::* callback)(TArg) const);

	private:
		using EventDeleter = void (*)(const Event*) noexcept;

		struct QueuedEvent
		{
			std::type_index type;
			std::unique_ptr<const Event, EventDeleter> event;
		};

		std::vector<QueuedEvent> writeBuffer;
		std::vector<QueuedEvent> readBuffer;
		std::unordered_map<std::type_index, std::vector<EventCallback>> listeners;
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
		const auto result = listeners.find(std::type_index(typeid(EventType)));
		if (result == listeners.end())
		{
			return;
		}

		const auto callbacks = result->second;
		for (const EventCallback& callback : callbacks)
		{
			callback(event);
		}
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
		const auto callbacks = result->second;
		for (const EventCallback& callback : callbacks)
		{
			callback(event);
		}
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::AddListener(std::function<void(const TEvent&)> callback)
	{
		ASSERT_FMT(callback, "이벤트 콜백이 비어 있습니다.");

		listeners[std::type_index(typeid(TEvent))].push_back(
			[cb = std::move(callback)](const Event& event)
			{
				cb(static_cast<const TEvent&>(event));
			});
	}

	template <std::derived_from<Event> TEvent, class TInstance, class TArg>
		requires std::invocable<void (TInstance::*)(TArg), TInstance*, const TEvent&>
	void EventSubsystem::AddListener(TInstance* instance, void (TInstance::* callback)(TArg))
	{
		ASSERT_FMT(instance, "이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "이벤트 콜백이 nullptr입니다.");

		AddListener<TEvent>(
			[instance, callback](const TEvent& event)
			{
				(instance->*callback)(event);
			});
	}

	template <std::derived_from<Event> TEvent, class TInstance, class TArg>
		requires std::invocable<void (TInstance::*)(TArg) const, const TInstance*, const TEvent&>
	void EventSubsystem::AddListener(const TInstance* instance, void (TInstance::* callback)(TArg) const)
	{
		ASSERT_FMT(instance, "이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "이벤트 콜백이 nullptr입니다.");

		AddListener<TEvent>(
			[instance, callback](const TEvent& event)
			{
				(instance->*callback)(event);
			});
	}
}
