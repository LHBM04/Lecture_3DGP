#pragma once

#include "EventBase.h"

class WindowMaximizeEvent  final : public EventBase
{
public:
    explicit WindowMaximizeEvent(Window& window_) noexcept
        : EventBase{ window_ }
    {
    }
};
