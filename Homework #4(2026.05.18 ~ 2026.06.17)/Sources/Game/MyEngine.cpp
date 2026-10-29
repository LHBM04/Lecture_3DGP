#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform.hpp"

DEFINE_ENGINE_INSTANCE(::TUK::Game::MyEngine)

namespace TUK::Game
{
	MyEngine::MyEngine() noexcept
	{
	}

	MyEngine::~MyEngine() noexcept
	{
	}

	SystemStatus MyEngine::Startup()
	{
		AddSubsystem<Framework::TimeSubsystem>(); 
		AddSubsystem<Framework::DisplaySubsystem>();
		Framework::RenderSubsystem* renderSubsystem = AddSubsystem<Framework::RenderSubsystem>();

		Framework::EventSubsystem* eventSubsystem = AddSubsystem<Framework::EventSubsystem>();
		eventSubsystem->AddListener<Framework::DisplayCreateData>(
			[renderSubsystem](const Framework::Display& display, const Framework::DisplayCreateData&)
			{
				renderSubsystem->RegisterDisplay(display);
			});
		eventSubsystem->AddListener<Framework::DisplayCloseData>(
			[renderSubsystem](const Framework::Display& display, const Framework::DisplayCloseData&)
			{
				renderSubsystem->UnregisterDisplay(display);
			});

		AddSubsystem<Framework::ConsoleSubsystem>();

		return Engine::Startup();
	}
}
