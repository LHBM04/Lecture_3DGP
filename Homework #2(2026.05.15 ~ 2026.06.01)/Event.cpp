#include "Precompiled.h"
#include "Event.h"

bool Event::IsHandled() const noexcept
{
	return handled;
}

void Event::SetHandled(bool value_) noexcept
{
	handled = value_;
}
