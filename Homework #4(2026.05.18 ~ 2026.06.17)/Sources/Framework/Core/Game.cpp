#include "Precompiled.hpp"
#include "Game.hpp"

#include "../Platform/TimeSubsystem.hpp"
#include "Engine.hpp"

namespace TUK::Framework
{
	Game::Game()
	{
	}

	Game::~Game()
	{
	}

	void Game::Begin()
	{
		Startup();
	}

	void Game::Tick()
	{
		auto time = Engine::GetInstance().GetSubsystem<TimeSubsystem>();

		const double deltaTime = time->GetDeltaTime();

		for (auto& subsystem : GetSubsystems())
		{
			if (!IsRunning())
			{
				return;
			}
			subsystem.EarlyUpdate(deltaTime);
		}

		while (IsRunning() && time->ConsumeFixedStep())
		{
			const double fixedDeltaTime = time->GetFixedDeltaTime();
			for (auto& subsystem : GetSubsystems())
			{
				if (!IsRunning())
				{
					return;
				}
				subsystem.FixedUpdate(fixedDeltaTime);
			}
		}

		for (auto& subsystem : GetSubsystems())
		{
			if (!IsRunning())
			{
				return;
			}
			subsystem.Update(deltaTime);
		}

		for (auto& subsystem : GetSubsystems())
		{
			if (!IsRunning())
			{
				return;
			}
			subsystem.LateUpdate(deltaTime);
		}
	}

	void Game::End()
	{
		Shutdown();
	}
}