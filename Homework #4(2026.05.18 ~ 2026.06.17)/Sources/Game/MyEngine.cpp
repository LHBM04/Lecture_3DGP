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
		eventSubsystem->AddListener<Framework::DisplayCreateEvent>(renderSubsystem, &Framework::RenderSubsystem::RegisterDisplay);
		eventSubsystem->AddListener<Framework::DisplayCloseEvent>(renderSubsystem, &Framework::RenderSubsystem::UnregisterDisplay);

		AddSubsystem<Framework::ConsoleSubsystem>();

		return Engine::Startup();
	}
}
