#include "Precompiled.hpp"
#include "Framework/Core/Game.hpp"

#include "../Platform/TimeSubsystem.hpp"

#include "Engine.hpp"

namespace TUK::Framework
{
	Game::Game() noexcept
	{
		assert(!instance);
		instance = this;
	}

	Game::~Game() noexcept
	{
		instance = nullptr;
	}

	void Game::Tick()
	{
		for (GameSubsystem& subsystem : GetSubsystems())
		{
			subsystem.EarlyUpdate();
		}

		// 시간이 있을 때만 가능해야 함.
		// 근데 Game이 TimeSubsystem을 알고 있어야 하는 게 맘에 안 듦.
		if (TimeSubsystem* const time = Engine::GetInstance()->GetSubsystem<TimeSubsystem>())
		{
			while (time->ShouldFixedStep())
			{
				for (GameSubsystem& subsystem : GetSubsystems())
				{
					subsystem.FixedUpdate();
				}
			}
		}

		for (GameSubsystem& subsystem : GetSubsystems())
		{
			subsystem.Update();
		}

		for (GameSubsystem& subsystem : GetSubsystems())
		{
			subsystem.LateUpdate();
		}
	}

	Game* Game::GetInstance()
	{
		assert(instance);
		return instance;
	}

	Game* Game::instance = nullptr;
}
