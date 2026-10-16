#include "Precompiled.hpp"
#include "Framework/Core/Engine.hpp"

#include "Game.hpp"

namespace TUK::Framework
{
	Engine::Engine() noexcept
	{
		assert(!instance);
		instance = this;
	}

	Engine::~Engine() noexcept
	{
		instance = nullptr;
	}

	void Engine::PreTick()
	{
		for (EngineSubsystem& subsystem : GetSubsystems())
		{
			subsystem.PreTick();
		}
	}

	void Engine::Tick()
	{
		for (auto& subsystem : GetSubsystems())
		{
			subsystem.Tick();
		}
	}

	void Engine::PostTick()
	{
		for (auto& subsystem : GetSubsystems())
		{
			subsystem.PostTick();
		}
	}

	Engine* Engine::GetInstance()
	{
		assert(instance);
		return instance;
	}

	Engine* Engine::instance = nullptr;
}