#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "Framework/Graphics/RenderSubsystem.hpp"

#include "Framework/Platform/DisplaySubsystem.hpp"
#include "Framework/Platform/TimeSubsystem.hpp"

DEFINE_ENGINE_INSTANCE(::TUK::Game::MyEngine)

namespace TUK::Game
{
	void MyEngine::Ready()
	{
		Engine::Ready();

		AddSubsystem<Framework::TimeSubsystem>();
		AddSubsystem<Framework::DisplaySubsystem>();
		AddSubsystem<Framework::RenderSubsystem>();
	}

	MyEngine::MyEngine() noexcept
	{
	}

	MyEngine::~MyEngine() noexcept
	{
	}
}
