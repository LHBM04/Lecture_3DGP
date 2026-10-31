#pragma once

#include <cassert>
#include <concepts>
#include <functional>
#include <memory>
#include <memory_resource>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../Core/EngineSubsystem.hpp"

#include "Event.hpp"

namespace TUK::Framework
{
	class EventSubsystem : public EngineSubsystem
	{
	public:
		using EventCallback = std::function<void(const Event&)>;

		explicit EventSubsystem() noexcept;
		~EventSubsystem() noexcept override = default;

		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

		template <std::derived_from<Event> TEvent, class... TArgs>
		void PushEvent(TArgs&&... args);

		template <std::derived_from<Event> TEvent>
		void AddListener(std::function<void(const TEvent&)> callback);

		template <std::derived_from<Event> TEvent, class TInstance, class TArg>
			requires std::convertible_to<const TEvent&, TArg>
		void AddListener(TInstance* instance, void (TInstance::* callback)(TArg));

		template <std::derived_from<Event> TEvent, class TInstance, class TArg>
			requires std::convertible_to<const TEvent&, TArg>
		void AddListener(const TInstance* instance, void (TInstance::* callback)(TArg) const);

	private:
		std::pmr::vector<std::unique_ptr<const Event>> writeBuffer;
		std::pmr::vector<std::unique_ptr<const Event>> readBuffer;
		std::pmr::unordered_map<std::type_index, std::vector<EventCallback>> listeners;
	};

	inline EventSubsystem::EventSubsystem() noexcept
		: EngineSubsystem(0)
	{
	}

	template <std::derived_from<Event> TEvent, class... TArgs>
	void EventSubsystem::PushEvent(TArgs&&... args)
	{
		writeBuffer.push_back(std::make_unique<const TEvent>(std::forward<TArgs>(args)...));
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::AddListener(std::function<void(const TEvent&)> callback)
	{
		assert(callback);
		listeners[std::type_index(typeid(TEvent))].push_back(
			[cb = std::move(callback)](const Event& event)
			{
				cb(static_cast<const TEvent&>(event));
			});
	}

	template <std::derived_from<Event> TEvent, class TInstance, class TArg>
		requires std::convertible_to<const TEvent&, TArg>
	void EventSubsystem::AddListener(TInstance* instance, void (TInstance::* callback)(TArg))
	{
		assert(instance && callback);
		AddListener<TEvent>(
			[instance, callback](const TEvent& event)
			{
				(instance->*callback)(event);
			});
	}

	template <std::derived_from<Event> TEvent, class TInstance, class TArg>
		requires std::convertible_to<const TEvent&, TArg>
	void EventSubsystem::AddListener(const TInstance* instance, void (TInstance::* callback)(TArg) const)
	{
		assert(instance && callback);
		AddListener<TEvent>(
			[instance, callback](const TEvent& event)
			{
				(instance->*callback)(event);
			});
	}
}
