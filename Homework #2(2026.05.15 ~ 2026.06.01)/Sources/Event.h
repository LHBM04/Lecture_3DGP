#pragma once

struct Event
{
    enum class Type : unsigned char
    {
        None,

        WindowClose,
        WindowResize,

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
            int keyCode;
        } key;

        struct
        {
            int x;
            int y;
        } mouseMove;

        struct
        {
            int button;
            int x;
            int y;
        } mouseButton;
    };
};
