#pragma once

#include "EventBase.h"

class WindowResizeEvent final : public EventBase
{
public:
    explicit WindowResizeEvent(Window& window_, int width_, int height_) noexcept
        : EventBase{ window_ }
        , width{ width_ }
        , height{ height_ }
    {
    }

    int width;
    int height;
};
