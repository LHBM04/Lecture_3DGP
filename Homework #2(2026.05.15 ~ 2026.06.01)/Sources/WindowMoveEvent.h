#pragma once

#include "EventBase.h"

class WindowMoveEvent final : public EventBase
{
public:
    explicit WindowMoveEvent(Window& window_, int x_, int y_) noexcept
        : EventBase{ window_ }
        , x{ x_ }
        , y{ y_ }
    {
    }

    int x;
    int y;
};