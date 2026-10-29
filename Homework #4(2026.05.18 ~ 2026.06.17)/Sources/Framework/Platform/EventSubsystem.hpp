#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <memory_resource>
#include <utility>
#include <variant>
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

		void PushEvent(Event event);

		template <class TData>
		void AddListener(std::function<void(const Display&, const TData&)> callback);

	protected:
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		void OnShutdown() override;

	private:
		using Listener = std::function<void(const Event&)>;

		std::pmr::vector<Event> writeBuffer;
		std::pmr::vector<Event> readBuffer;
		std::array<std::vector<Listener>, std::variant_size_v<EventData>> listeners;
	};

	template <class TData>
	void EventSubsystem::AddListener(std::function<void(const Display&, const TData&)> callback)
	{
		assert(callback);
		constexpr std::size_t EVENT_INDEX = EventData{ std::in_place_type<TData> }.index();
		listeners[EVENT_INDEX].push_back(
			[cb = std::move(callback)](const Event& event)
			{
				const TData* data = std::get_if<TData>(&event.data);
				assert(data);
				cb(event.display, *data);
			});
	}
}
