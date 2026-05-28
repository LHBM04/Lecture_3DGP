#pragma once

#include "EventBase.h"

class WindowMinimizeEvent final : public EventBase
{
public:
    explicit WindowMinimizeEvent(Window& window_) noexcept
        : EventBase{ window_ }
    {
    }
};
