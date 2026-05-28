#pragma once

#include "Event.h"

class WindowMinimizeEvent final : public Event
{
public:
    explicit WindowMinimizeEvent(Window* window_) noexcept
        : Event{ window_ }
    {
    }
};
