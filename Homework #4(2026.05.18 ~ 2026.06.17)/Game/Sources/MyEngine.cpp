#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "Framework/Graphics/RenderSubsystem.hpp"

#include "Framework/Platform/DisplaySubsystem.hpp"
#include "Framework/Platform/EventSubsystem.hpp"
#include "Framework/Platform/TimeSubsystem.hpp"

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
		AddSubsystem<Framework::EventSubsystem>();
		AddSubsystem<Framework::DisplaySubsystem>();
		AddSubsystem<Framework::RenderSubsystem>();

		return SystemStatus::Continue;
	}
}
