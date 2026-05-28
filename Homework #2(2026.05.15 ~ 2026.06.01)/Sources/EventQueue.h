#pragma once

#include "Event.h"

#include <memory>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

class EventQueue
{
public:
    EventQueue() = default;

    EventQueue(const EventQueue&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;

    EventQueue(EventQueue&&) noexcept = default;
    EventQueue& operator=(EventQueue&&) noexcept = default;

    template <typename TEvent, typename... Args>
    TEvent& Push(Args&&... args)
    {
        static_assert(std::is_base_of_v<Event, TEvent>);

        auto event = std::make_unique<TEvent>(std::forward<Args>(args)...);
        TEvent& result = *event;

        events.emplace_back(std::move(event));
        return result;
    }

    void Push(std::unique_ptr<Event> event)
    {
        if (event == nullptr)
        {
            return;
        }

        events.emplace_back(std::move(event));
    }

    void Clear() noexcept;
    bool IsEmpty() const noexcept;
    std::size_t GetCount() const noexcept;

    std::span<std::unique_ptr<Event>> GetEvents() noexcept;
    std::span<const std::unique_ptr<Event>> GetEvents() const noexcept;

private:
    std::vector<std::unique_ptr<Event>> events;
};
