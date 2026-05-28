#pragma once

#include "Event.h"

class WindowMaximizeEvent final : public Event
{
public:
    explicit WindowMaximizeEvent(Window* window_) noexcept
        : Event{ window_ }
    {
    }
};
