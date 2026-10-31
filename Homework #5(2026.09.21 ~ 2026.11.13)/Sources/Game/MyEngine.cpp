#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "../Framework/Platform/DisplaySubsystem.hpp"
#include "../Framework/Platform/EventSubsystem.hpp"

namespace TUK::Game
{
	SystemStatus MyEngine::Startup()
	{
		AddSubsystem<EventSubsystem>();
		AddSubsystem<DisplaySubsystem>();

		return Engine::Startup();
	}
}
