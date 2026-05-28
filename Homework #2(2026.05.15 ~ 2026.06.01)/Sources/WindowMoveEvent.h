#pragma once

#include "Event.h"

class WindowMoveEvent final : public Event
{
public:
    WindowMoveEvent(Window* window_, int x_, int y_) noexcept
        : Event{ window_ }
        , x{ x_ }
        , y{ y_ }
    {
    }

    int x{};
    int y{};
};
