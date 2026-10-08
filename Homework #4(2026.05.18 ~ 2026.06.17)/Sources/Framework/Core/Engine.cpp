#include "Precompiled.hpp"
#include "Engine.hpp"

#include "Game.hpp"

namespace TUK::Framework
{
	Engine::Engine()
	{
	}

	Engine::~Engine()
	{
	}

	Engine& Engine::GetInstance()
	{
		assert(instance);
		return *instance;
	}

	int Engine::Run(Game& game)
	{
		assert(!instance);
		instance = this;
		Startup();

		if (IsRunning())
		{
			game.Begin();

			if (!game.IsRunning())
			{
				RequestQuit(game.GetQuitCode());
			}

			while (IsRunning())
			{
				OnPreTick();
				OnTick(game);
				OnPostTick();

				if (!game.IsRunning())
				{
					RequestQuit(game.GetQuitCode());
				}
			}

			// 게임 자원을 먼저 해제한 후 엔진 서비스를 종료한다.
			game.End();
			if (game.GetQuitCode() != EXIT_SUCCESS)
			{
				RequestQuit(game.GetQuitCode());
			}
		}

		Shutdown();
		instance = nullptr;
		return GetQuitCode();
	}

	void Engine::OnPreTick()
	{
		for (auto& subsystem : GetSubsystems<EngineSubsystem>())
		{
			subsystem.OnPreTick();
		}
	}

	void Engine::OnTick(Game& game)
	{
		for (auto& subsystem : GetSubsystems<EngineSubsystem>())
		{
			subsystem.OnTick();
		}

		if (IsRunning())
		{
			game.Tick();
		}
	}

	void Engine::OnPostTick()
	{
		for (auto& subsystem : GetSubsystems<EngineSubsystem>())
		{
			subsystem.OnPostTick();
		}
	}

	Engine* Engine::instance = nullptr;
}