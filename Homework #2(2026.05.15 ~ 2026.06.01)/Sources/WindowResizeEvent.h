#pragma once

#include "Event.h"

class WindowResizeEvent final : public Event
{
public:
    WindowResizeEvent(Window* window_, int width_, int height_) noexcept
        : Event{ window_ }
        , width{ width_ }
        , height{ height_ }
    {
    }

    int width{};
    int height{};
};
