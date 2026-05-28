#pragma once

#include <concepts>
#include <memory>
#include <span>
#include <type_traits>
#include <vector>

#include "EventBase.h"

class EventQueue
{
public:
    EventQueue() = default;
    ~EventQueue() = default;

    EventQueue(const EventQueue&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;

    EventQueue(EventQueue&&) noexcept = default;
    EventQueue& operator=(EventQueue&&) noexcept = default;

    template <std::derived_from<EventBase> TEvent, class... TArgs>
    TEvent& Push(TArgs&&... args_);

    auto GetEvents(this const auto& self_) noexcept;

private:
    std::vector<std::unique_ptr<EventBase>> events;
};

template <std::derived_from<EventBase> TEvent, class... TArgs>
inline TEvent& EventQueue::Push(TArgs&&... args_)
{
    static_assert(std::is_base_of_v<TEvent, TEvent>);

    auto event = std::make_unique<TEvent>(std::forward<TArgs>(args_)...);
    TEvent& result = *event;

    events.emplace_back(std::move(event));
    return result;
}

auto EventQueue::GetEvents(this const auto& self_) noexcept
{
    return self_.events
        | std::views::transform([](const std::unique_ptr<EventBase>& event_) noexcept -> const EventBase*
            {
                return event_.get();
            });
}
