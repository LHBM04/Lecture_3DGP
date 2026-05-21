#pragma once

#include "Event.h"
#include "MouseButton.h"

class MouseMoveEvent final : public Event
{
public:
	int x{ 0 };
	int y{ 0 };
};

class MouseButtonEvent : public Event
{
public:
	MouseButton button{ MouseButton::Left };
	int x{ 0 };
	int y{ 0 };
};

class MouseButtonDownEvent final : public MouseButtonEvent
{
};

class MouseButtonUpEvent final : public MouseButtonEvent
{
};
