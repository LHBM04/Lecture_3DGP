#pragma once

#include "EventBase.h"

class WindowCloseEvent : public EventBase
{
public:
    explicit WindowCloseEvent(Window& window_) noexcept
        : EventBase{ window_ }
    {
    }
};
