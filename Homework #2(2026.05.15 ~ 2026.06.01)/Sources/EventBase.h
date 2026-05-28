#pragma once

#include <cassert>
#include <type_traits>

class Window;

class EventBase
{
public:
    explicit EventBase(Window& window_)
        : window(window_)
    {
    }

	virtual ~EventBase() = default;

    template <class TEvent>
    bool Is(this const EventBase& self_) noexcept;

    template <class TEvent, class TSelf>
    auto As(this TSelf&& self_) noexcept;

private:
    Window& window;
};

template <class TEvent>
inline bool EventBase::Is(this const EventBase& self_) noexcept
{
    static_assert(std::is_base_of_v<EventBase, TEvent>);

    return dynamic_cast<const TEvent*>(this) != nullptr;
}

template <class TEvent, class TSelf>
inline auto EventBase::As(this TSelf&& self_) noexcept
{
    static_assert(std::is_base_of_v<EventBase, TEvent>);
    static_assert(Is<TEvent>());

    return static_cast<TSelf&&>(*this);
}
