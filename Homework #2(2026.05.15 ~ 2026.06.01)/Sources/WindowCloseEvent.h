#pragma once

#include "Event.h"

class WindowCloseEvent final : public Event
{
public:
    explicit WindowCloseEvent(Window* window_) noexcept
        : Event{ window_ }
    {
    }
};
