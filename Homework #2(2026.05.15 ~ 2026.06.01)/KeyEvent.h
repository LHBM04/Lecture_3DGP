#pragma once

#include "Event.h"

class KeyEvent : public Event
{
public:
	int keyCode{ 0 };
	bool isRepeat{ false };
};

class KeyDownEvent final : public KeyEvent
{
};

class KeyUpEvent final : public KeyEvent
{
};
