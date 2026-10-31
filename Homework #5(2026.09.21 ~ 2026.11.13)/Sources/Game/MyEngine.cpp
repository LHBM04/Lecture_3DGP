#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "../Framework/Platform.hpp"
#include "../Framework/Graphics.hpp"

namespace TUK::Game
{
	SystemStatus MyEngine::Startup()
	{
		AddSubsystem<DisplaySubsystem>();

		EventSubsystem* eventSubsystem = AddSubsystem<EventSubsystem>();
		
		RenderSubsystem* renderSubsystem = AddSubsystem<RenderSubsystem>();
		eventSubsystem->AddListener<DisplayCreateEvent>(renderSubsystem, &RenderSubsystem::AddDisplay);
		eventSubsystem->AddListener<DisplayCloseEvent>(renderSubsystem, &RenderSubsystem::RemoveDisplay);

		return Engine::Startup();
	}
}
