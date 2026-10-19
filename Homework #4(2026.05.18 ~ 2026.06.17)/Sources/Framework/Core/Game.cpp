#include "Precompiled.hpp"
#include "Game.hpp"

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

	SystemStatus Game::Routine()
	{
		SystemStatus next = SystemStatus::Continue;

		if (SystemStatus current = EarlyUpdate(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		if (SystemStatus current = FixedUpdate(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		if (SystemStatus current = Update(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}
		
		if (SystemStatus current = LateUpdate(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus Game::EarlyUpdate()
	{
		for (GameSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnEarlyUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Success;
	}

	SystemStatus Game::FixedUpdate()
	{
		for (GameSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnFixedUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Success;
	}

	SystemStatus Game::Update()
	{
		for (GameSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Success;
	}

	SystemStatus Game::LateUpdate()
	{
		for (GameSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnLateUpdate();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Success;
	}

	Game* Game::GetInstance()
	{
		assert(instance);
		return instance;
	}

	Game* Game::instance = nullptr;
}
