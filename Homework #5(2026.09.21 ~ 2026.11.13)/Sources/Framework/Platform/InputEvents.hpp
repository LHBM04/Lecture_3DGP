#pragma once

#include "Event.hpp"

namespace TUK::Framework
{
	struct InputActivationEvent : Event
	{
		bool isActive = false;
	};
}
