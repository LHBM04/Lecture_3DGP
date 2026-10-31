#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "../Framework/Platform.hpp"

namespace TUK::Game
{
	SystemStatus MyEngine::Startup()
	{
		AddSubsystem<EventSubsystem>();
		AddSubsystem<DisplaySubsystem>();

		return Engine::Startup();
	}
}
