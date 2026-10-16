#include "Precompiled.hpp"
#include "MyEngine.hpp"

#include "Framework/Graphics/RenderSubsystem.hpp"

#include "Framework/Platform/TimeSubsystem.hpp"
#include "Framework/Platform/WindowSubsystem.hpp"

DEFINE_ENGINE_INSTANCE(::TUK::Game::MyEngine)

namespace TUK::Game
{
	void MyEngine::Ready()
	{
		Engine::Ready();

		AddSubsystem<Framework::TimeSubsystem>();
		AddSubsystem<Framework::WindowSubsystem>();
		AddSubsystem<Framework::RenderSubsystem>();
	}

	MyEngine::MyEngine() noexcept
	{
	}

	MyEngine::~MyEngine() noexcept
	{
	}
}
