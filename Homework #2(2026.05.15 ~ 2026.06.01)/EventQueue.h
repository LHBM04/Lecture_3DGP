#pragma once

#include "Event.h"

class EventQueue final
{
public:
	EventQueue() noexcept = default;
	~EventQueue() noexcept = default;

	EventQueue(const EventQueue&) = delete;
	EventQueue& operator=(const EventQueue&) = delete;

	EventQueue(EventQueue&&) = delete;
	EventQueue& operator=(EventQueue&&) = delete;

	template <class TEvent>
	void Push(TEvent event_);

	template <class TEvent>
	[[nodiscard]] std::span<TEvent> GetEvents() noexcept;

	template <class TEvent>
	[[nodiscard]] std::span<const TEvent> GetEvents() const noexcept;

	template <class TEvent, class TFunction>
	void ForEach(TFunction&& function_);

	template <class TEvent, class TFunction>
	void ForEach(TFunction&& function_) const;

	void Clear() noexcept;

private:
	struct IStorage
	{
		IStorage() noexcept = default;
		virtual ~IStorage() noexcept = default;

		IStorage(const IStorage&) = delete;
		IStorage& operator=(const IStorage&) = delete;

		IStorage(IStorage&&) = delete;
		IStorage& operator=(IStorage&&) = delete;
	};

	template <class TEvent>
	struct Storage final : IStorage
	{
		Storage() noexcept = default;
		~Storage() noexcept override = default;

		std::vector<TEvent> events;
	};

	template <class TEvent>
	[[nodiscard]] std::vector<TEvent>& GetOrCreateStorage();

	template <class TEvent>
	[[nodiscard]] std::vector<TEvent>& GetStorage() noexcept;

	template <class TEvent>
	[[nodiscard]] const std::vector<TEvent>& GetStorage() const noexcept;

	std::unordered_map<std::type_index, std::unique_ptr<IStorage>> storages;
	std::vector<std::function<void()>> clearers;
};

template <class TEvent>
inline void EventQueue::Push(TEvent event_)
{
	GetOrCreateStorage<TEvent>().emplace_back(std::move(event_));
}

template <class TEvent>
inline std::span<TEvent> EventQueue::GetEvents() noexcept
{
	std::vector<TEvent>& events{ GetStorage<TEvent>() };
	return std::span<TEvent>{ events.data(), events.size() };
}

template <class TEvent>
inline std::span<const TEvent> EventQueue::GetEvents() const noexcept
{
	const std::vector<TEvent>& events{ GetStorage<TEvent>() };
	return std::span<const TEvent>{ events.data(), events.size() };
}

template <class TEvent, class TFunction>
inline void EventQueue::ForEach(TFunction&& function_)
{
	for (TEvent& event : GetEvents<TEvent>())
	{
		std::invoke(std::forward<TFunction>(function_), event);
	}
}

template <class TEvent, class TFunction>
inline void EventQueue::ForEach(TFunction&& function_) const
{
	for (const TEvent& event : GetEvents<TEvent>())
	{
		std::invoke(std::forward<TFunction>(function_), event);
	}
}

template <class TEvent>
inline std::vector<TEvent>& EventQueue::GetOrCreateStorage()
{
	const std::type_index key{ typeid(TEvent) };
	const auto iterator{ storages.find(key) };
	if (iterator != storages.end())
	{
		return static_cast<Storage<TEvent>&>(*iterator->second).events;
	}

	auto storage{ std::make_unique<Storage<TEvent>>() };
	Storage<TEvent>* const rawStorage{ storage.get() };

	storages.emplace(key, std::move(storage));
	clearers.emplace_back(
		[rawStorage]()
		{
			rawStorage->events.clear();
		});

	return rawStorage->events;
}

template <class TEvent>
inline std::vector<TEvent>& EventQueue::GetStorage() noexcept
{
	const std::type_index key{ typeid(TEvent) };
	const auto iterator{ storages.find(key) };
	if (iterator == storages.end())
	{
		static std::vector<TEvent> empty{};
		return empty;
	}

	return static_cast<Storage<TEvent>&>(*iterator->second).events;
}

template <class TEvent>
inline const std::vector<TEvent>& EventQueue::GetStorage() const noexcept
{
	const std::type_index key{ typeid(TEvent) };
	const auto iterator{ storages.find(key) };
	if (iterator == storages.end())
	{
		static const std::vector<TEvent> empty{};
		return empty;
	}

	return static_cast<const Storage<TEvent>&>(*iterator->second).events;
}
