#pragma once

#include "Event.h"

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

class EventDispatcher
{
public:
    explicit EventDispatcher(Event& event_) noexcept
        : event{ event_ }
    {
    }

    template <typename TEvent, typename TCallback>
    bool Dispatch(TCallback&& callback)
    {
        static_assert(std::is_base_of_v<Event, TEvent>);

        if (!event.Is<TEvent>())
        {
            return false;
        }

        TEvent& typedEvent = event.As<TEvent>();

        if constexpr (std::same_as<std::invoke_result_t<TCallback, TEvent&>, bool>)
        {
            return std::forward<TCallback>(callback)(typedEvent);
        }
        else
        {
            std::forward<TCallback>(callback)(typedEvent);
            return true;
        }
    }

private:
    Event& event;
};
