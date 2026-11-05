#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "Framework/Platform.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Utilities.hpp"

namespace TUK::Game
{
	SystemStatus MyEngine::Startup()
	{
		displaySubsystem = AddSubsystem<DisplaySubsystem>();
		if (!displaySubsystem)
		{
			Logger::LogError("Failed to add DisplaySubsystem.");
			return SystemStatus::FAILURE;
		}

		eventSubsystem = AddSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			Logger::LogError("Failed to add EventSubsystem.");
			return SystemStatus::FAILURE;
		}
		
		renderSubsystem = AddSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			Logger::LogError("Failed to add RenderSubsystem.");
			return SystemStatus::FAILURE;
		}

		// Register event listeners for display events
		eventSubsystem->AddListener<DisplayCreateEvent>(renderSubsystem, &RenderSubsystem::OnDisplayCreate);
		eventSubsystem->AddListener<DisplayCloseEvent>(renderSubsystem, &RenderSubsystem::OnDisplayClose);

		return Engine::Startup();
	}
}
