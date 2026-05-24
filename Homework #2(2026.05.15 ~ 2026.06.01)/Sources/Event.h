#pragma once

#include "InputManager.h"

struct Event
{
    enum class Type : unsigned char
    {
        None,

        WindowClose,
        WindowResize,
        WindowFullscreenToggle,

        KeyDown,
        KeyUp,

        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
    };

    Type type;

    union
    {
        struct
        {
            int width;
            int height;
        } resize;

        struct
        {
            KeyCode keyCode;
        } key;

        struct
        {
            int x;
            int y;
        } mouseMove;

        struct
        {
            ButtonCode button;
            int x;
            int y;
        } mouseButton;
    };
};
