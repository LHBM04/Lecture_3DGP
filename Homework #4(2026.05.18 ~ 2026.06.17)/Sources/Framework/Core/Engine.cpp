#include "Precompiled.hpp"
#include "Engine.hpp"

#include "Game.hpp"
#include "Memory.hpp"

namespace TUK::Framework
{
	Engine::Engine() noexcept
		: game(nullptr)
	{
		assert(!instance);
		instance = this;
	}

	Engine::~Engine() noexcept
	{
		instance = nullptr;
	}

	std::int32_t Engine::Run(Game& game)
	{
		this->game = &game;

		Startup();
		this->game->Startup();

		while (true)
		{
			if (SystemStatus status = Routine(); status != SystemStatus::Continue)
			{
				this->game->Shutdown();
				Shutdown();

				return static_cast<std::int32_t>(status);
			}
		}
	}

	SystemStatus Engine::Routine()
	{
		SystemStatus next = SystemStatus::Continue;

		if (SystemStatus current = PreTick(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		if (SystemStatus current = Tick(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		if (SystemStatus current = game->Routine(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		if (SystemStatus current = PostTick(); current != SystemStatus::Continue)
		{
			if (current != SystemStatus::Continue)
			{
				if (next == SystemStatus::Continue)
				{
					next = current;
				}
			}
		}

		return next;
	}

	SystemStatus Engine::PreTick()
	{
		for (EngineSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnPreTick();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus Engine::Tick()
	{
		for (EngineSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnTick();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Continue;
	}

	SystemStatus Engine::PostTick()
	{
		for (EngineSubsystem& subsystem : GetSubsystems())
		{
			const SystemStatus status = subsystem.OnPostTick();
			if (status != SystemStatus::Continue)
			{
				return status;
			}
		}

		return SystemStatus::Continue;
	}

	Engine* Engine::GetInstance()
	{
		assert(instance);
		return instance;
	}

	Engine* Engine::instance = nullptr;
}
