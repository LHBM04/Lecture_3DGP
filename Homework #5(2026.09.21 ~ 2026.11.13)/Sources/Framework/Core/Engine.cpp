#include "Precompiled.hpp"
#include "Engine.hpp"

#include "App.hpp"

namespace TUK::Framework
{
	i32 Engine::Run(App* target)
	{
		app = target;

		if (const SystemStatus status = Startup(); status != SystemStatus::CONTINUE)
		{
			Shutdown();
			return std::to_underlying(status);
		}
		if (const SystemStatus status = app->Startup(); status != SystemStatus::CONTINUE)
		{
			app->Shutdown();
			Shutdown();
			return std::to_underlying(status);
		}

		while (true)
		{
			if (SystemStatus status = Routine(); status != SystemStatus::CONTINUE)
			{
				app->Shutdown();
				Shutdown();

				return std::to_underlying(status);
			}
		}

		std::unreachable();
	}

	SystemStatus Engine::Startup()
	{
		instance = this;
		return System::Startup();
	}

	SystemStatus Engine::Routine()
	{
		if (SystemStatus status = PreTick(); status != SystemStatus::CONTINUE)
		{
			return status;
		}

		if (SystemStatus status = Tick(); status != SystemStatus::CONTINUE)
		{
			return status;
		}

		if (SystemStatus status = app->Routine(); status != SystemStatus::CONTINUE)
		{
			return status;
		}

		if (SystemStatus status = PostTick(); status != SystemStatus::CONTINUE)
		{
			return status;
		}

		return SystemStatus::CONTINUE;
	}

	void Engine::Shutdown()
	{
		System::Shutdown();
		instance = nullptr;
	}

	SystemStatus Engine::PreTick()
	{
		for (EngineSubsystem* subsystem : GetSubsystems())
		{
			if (const SystemStatus status = subsystem->OnPreTick(); status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus Engine::Tick()
	{
		for (EngineSubsystem* subsystem : GetSubsystems())
		{
			if (const SystemStatus status = subsystem->OnTick(); status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus Engine::PostTick()
	{
		for (EngineSubsystem* subsystem : GetSubsystems())
		{
			if (const SystemStatus status = subsystem->OnPostTick(); status != SystemStatus::CONTINUE)
			{
				return status;
			}
		}

		return SystemStatus::CONTINUE;
	}
}
