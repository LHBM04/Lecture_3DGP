#pragma once

#include <concepts>
#include <functional>
#include <memory>
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
		EventSubsystem() noexcept;
		~EventSubsystem() noexcept override;

		template <std::derived_from<Event> TEvent, class... TArgs>
		void PushEvent(TArgs&&... args);

		void PushEvent(std::unique_ptr<const Event> event);

		template <std::derived_from<Event> TEvent>
		void Subscribe(std::function<void(const TEvent&)> callback);

	protected:
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		void OnShutdown() override;

	private:
		using Listener = std::function<void(const Event&)>;

		std::vector<std::unique_ptr<const Event>> writeBuffer;
		std::vector<std::unique_ptr<const Event>> readBuffer;
		std::unordered_map<std::type_index, std::vector<Listener>> listeners;
	};

	template <std::derived_from<Event> TEvent, class... TArgs>
	void EventSubsystem::PushEvent(TArgs&&... args)
	{
		writeBuffer.push_back(std::make_unique<const TEvent>(std::forward<TArgs>(args)...));
	}

	template <std::derived_from<Event> TEvent>
	void EventSubsystem::Subscribe(std::function<void(const TEvent&)> callback)
	{
		listeners[typeid(TEvent)].push_back(
			[cb = std::move(callback)](const Event& event)
			{
				cb(static_cast<const TEvent&>(event));
			});
	}
}
