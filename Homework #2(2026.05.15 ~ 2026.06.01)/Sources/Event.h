#pragma once

#include <cassert>
#include <type_traits>

class Window;

class Event
{
public:
    explicit Event(Window* window_) noexcept
        : window{ window_ }
    {
    }

    virtual ~Event() noexcept = default;

    [[nodiscard]]
    Window* GetWindow() const noexcept
    {
        return window;
    }

    template <typename TEvent>
    bool Is() const noexcept;

    template <typename TEvent>
    TEvent& As() noexcept;

    template <typename TEvent>
    const TEvent& As() const noexcept;

private:
    Window* window{};
};

template <typename TEvent>
inline bool Event::Is() const noexcept
{
    static_assert(std::is_base_of_v<Event, TEvent>);
    return dynamic_cast<const TEvent*>(this) != nullptr;
}

template <typename TEvent>
inline TEvent& Event::As() noexcept
{
    static_assert(std::is_base_of_v<Event, TEvent>);

    assert(Is<TEvent>());
    return static_cast<TEvent&>(*this);
}

template <typename TEvent>
inline const TEvent& Event::As() const noexcept
{
    static_assert(std::is_base_of_v<Event, TEvent>);

    assert(Is<TEvent>());
    return static_cast<const TEvent&>(*this);
}
