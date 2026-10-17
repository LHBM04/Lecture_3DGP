#include "Precompiled.hpp"
#include "MyEngine.hpp"

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

	SystemStatus MyEngine::Ready()
	{
		Engine::Ready();

		AddSubsystem<Framework::TimeSubsystem>();
		Framework::EventSubsystem* eventSubsystem = AddSubsystem<Framework::EventSubsystem>();
		AddSubsystem<Framework::DisplaySubsystem>();
		Framework::RenderSubsystem* renderSubsystem = AddSubsystem<Framework::RenderSubsystem>();

		eventSubsystem->Subscribe<Framework::DisplayCreateEvent>(renderSubsystem, &Framework::RenderSubsystem::CreateSwapChain);

		return SystemStatus::Continue;
	}
}
